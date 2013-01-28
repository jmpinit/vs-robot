#include <joyos.h>
#include <math.h>
#include "inc/sensors.h"
#include "inc/util_math.h"
#include "inc/control.h"
#include "inc/debug.h"
#include "inc/manager.h"

#define HIST_SHARP	10

territory arena[6] = {
	//center		mine			capture			mine angle	capture angle
	{{-1350, 20},	{-1620, 320},	{-1750, -375},	0,			28,				0,0,0},
	{{-625, -1060},	{-1100, -1230},	{-550, -1700},	0,			-28,			0,0,0},
	{{650, -1065},	{510, -1575}, 	{1200, -1300},	0,			-28,			0,0,0},
	{{1300, 30},	{1600, -400},	{1700, 330},	0,			-25,			0,0,0},
	{{640, 1165},	{1100, 1250},	{600, 1750},	0,			-25,			0,0,0},
	{{-700, 1200},	{-520, 1600},	{-1200, 1400},	0,			-25,			0,0,0}
};

enemy other_bot;

int vps_derivative;
pt vps_last;

float x, y;
long last_time;
pt last_pos;

int sensor(void) {
	last_time = get_time_us();

	if(vps_is_shit()) {
		x = arena[1].center.x;
		y = arena[1].center.y;
	} else {
		x = vps_x;
		y = vps_y;

		vps_last.x = vps_x;
		vps_last.y = vps_y;
	}

	last_pos.x = (int)x;
	last_pos.y = (int)y;

	while(true) {
		/* VPS */
		vps_update();

		vps_derivative = (int)distance(vps_x, vps_y, vps_last.x, vps_last.y);

		vps_last.x = vps_x;
		vps_last.y = vps_y;

		/* ENVIRONMENT */
		for(unsigned char i=0; i<6; i++) {
			arena[i].balls = vps_get_balls(i);
			arena[i].owner = vps_get_owner(i);
			arena[i].rate = vps_get_rate(i);
		}

		/* ORIENTATION */
		if(bot.velocity < 1 && abs(bot.heading-bot.target_heading)<8.0 && !vps_is_shit()) gyro_zero();
		bot.heading = gyro_absolute();

		/* POSITION */
		if(vps_is_shit()) {
			float dt = (get_time_us()-last_time)/1000000.0;
			float ticks = bot.velocity * VEL_SLOPE;
			float vps = encoder_to_vps(ticks);
			float d = vps*dt;

			x += d*cos(2.0*M_PI*bot.heading/360.0);
			y += d*sin(2.0*M_PI*bot.heading/360.0);
		} else {
			x = vps_x;
			y = vps_y;

			//keep track of enemy too
			other_bot.x = vps_enemy_x;
			other_bot.y = vps_enemy_y;
		}
		bot.x = (int)x;
		bot.y = (int)y;

		//printf("%d %d\n", bot.x, bot.y);

		last_time = get_time_us();

		/* REAL(ISH) VELOCITY */
		int dx = bot.x-last_pos.x;
		int dy = bot.y-last_pos.y;
		bot.real_velocity = distance(0, 0, dx, dy);

		/* OBSTRUCTION */
		bot.obstructed = false;
		if(bot.velocity>1 && bot.real_velocity<5) bot.obstructed = true;

		yield();
	}

	return 0;
}

void sense_init(void) {
	create_thread(&sensor, STACK_DEFAULT, 0, "sense_thread");
}

void gyro_zero(void) {
	bot.gyro_transform = within(-180, gyro_get_degrees() - vps_get_degrees(), 180);
}

float gyro_absolute(void) {
	return within(-180, gyro_get_degrees() - bot.gyro_transform, 180);
}

float vps_get_degrees(void) {
	float val = vps_heading;
	val /= 11.375;	//convert to degrees
	return val;
}

unsigned int vps_get_owner(unsigned char id) {
	if(id<6) return game.territories[id].owner;
	return 0;
}

unsigned int vps_get_balls(unsigned char id) {
	if(id<6) return game.territories[id].remaining;
	return 0;
}

unsigned int vps_get_rate(unsigned char id) {
	if(id<6) return game.territories[id].rate_limit;
	return 0;
}

bool vps_is_shit(void) {
	return (vps_x==0 && vps_y==0 && vps_heading==0) || (vps_derivative==0);
}

float motor_get_current_avg(void) {
	return (motor_get_current(MOTOR_LEFT)+motor_get_current(MOTOR_RIGHT))/2.0;
}

float encoder_read_avg(void) {
	return (encoder_read(ENCODER_LEFT)+encoder_read(ENCODER_RIGHT))/2.0;
}

float vps_to_encoder(float dist) {
	return dist*TICKS_PER_VPS;
}

float encoder_to_vps(int ticks) {
	return ticks/TICKS_PER_VPS;
}

unsigned char get_territory(int x, int y) {
	unsigned char id = 0;
	float angle = within(-180, 360*atan2(y, x)/(2.0*M_PI), 180);
	if(angle>150||angle<-150)
		id = 0;
	else if(angle>-150 && angle<-90)
		id = 1;
	else if(angle>-90 && angle<-30)
		id = 2;
	else if(angle>-30 && angle<30)
		id = 3;
	else if(angle>30 && angle<90)
		id = 4;
	else if(angle>90 && angle<150)
		id = 5;

	return id;
}
