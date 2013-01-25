#include <joyos.h>
#include <math.h>
#include "inc/sensors.h"
#include "inc/util_math.h"
#include "inc/control.h"
#include "inc/manager.h"

#define HIST_SHARP	10

typedef struct {
	float sharp[3];
} bot_senses;

bot_senses senses;

float sharp_left[HIST_SHARP];
float sharp_right[HIST_SHARP];
float sharp_back[HIST_SHARP];

float x, y;
long last_time;
pt last_pos;

int sensor(void) {
	last_time = get_time_us();

	if(vps_is_shit()) {
		x = map[1].center.x;
		y = map[1].center.y;
	} else {
		x = vps_x;
		y = vps_y;
	}

	last_pos.x = (int)x;
	last_pos.y = (int)y;

	while(true) {
		/* VPS */
		vps_update();

		/* ORIENTATION */
		if(bot.velocity < 1 && abs(bot.heading-bot.target_heading)<8.0 && !vps_is_shit()) gyro_zero();
		bot.heading = gyro_absolute();

		/* POSITION */
		if(vps_is_shit()) {
			float dt = (get_time_us()-last_time)/1000000.0;
			float ticks = bot.velocity * VEL_SLOPE;
			float vps = encoder_to_vps(ticks);
			float d = vps*dt;

			//printf("%f %f %f %f\n", dt, ticks, vps, d);

			x += d*cos(2.0*M_PI*bot.heading/360.0);
			y += d*sin(2.0*M_PI*bot.heading/360.0);
		} else {
			x = vps_x;
			y = vps_y;
		}
		bot.x = (int)x;
		bot.y = (int)y;

		last_time = get_time_us();

		/* REAL(ISH) VELOCITY */
		int dx = bot.x-last_pos.x;
		int dy = bot.y-last_pos.y;
		bot.real_velocity = distance(0, 0, dx, dy);

		/* OBSTRUCTION */
		bot.obstructed = false;
		if(bot.velocity>1 && bot.real_velocity<5) bot.obstructed = true;

		/* SHARP DISTANCE SENSORS */
		//move back histories
		for(int i=HIST_SHARP-1; i>0; i--)
			sharp_left[i] = sharp_left[i-1];
		for(int i=HIST_SHARP-1; i>0; i--)
			sharp_right[i] = sharp_left[i-1];
		for(int i=HIST_SHARP-1; i>0; i--)
			sharp_back[i] = sharp_left[i-1];

		//add new values
		sharp_left[0] = analog_read(SHARP_LEFT);
		sharp_right[0] = analog_read(SHARP_RIGHT);
		sharp_back[0] = analog_read(SHARP_BACK);

		//running average of distances
		float total = 0;
		for(int i=0; i<HIST_SHARP; i++)
			total += sharp_left[i];
		senses.sharp[0] = total/((float)HIST_SHARP);

		total = 0;
		for(int i=0; i<HIST_SHARP; i++)
			total += sharp_right[i];
		senses.sharp[1] = total/((float)HIST_SHARP);

		total = 0;
		for(int i=0; i<HIST_SHARP; i++)
			total += sharp_back[i];
		senses.sharp[2] = total/((float)HIST_SHARP);

		yield();
	}

	return 0;
}

float sharp_get_avg(unsigned char id) {
	unsigned int i;

	switch(id) {
		case SHARP_LEFT:
			i = 0;
			break;
		case SHARP_RIGHT:
			i = 1;
			break;
		case SHARP_BACK:
			i = 2;
			break;
		default:
			i = 0;
	}

	return senses.sharp[i];
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

unsigned char vps_get_owner(unsigned char id) {
	if(id<6) return game.territories[id].owner;
	return 0;
}

unsigned char vps_get_balls(unsigned char id) {
	if(id<6) return game.territories[id].remaining;
	return 0;
}

unsigned char vps_get_rate(unsigned char id) {
	if(id<6) return game.territories[id].rate_limit;
	return 0;
}

bool vps_is_shit(void) {
	#ifdef NO_VPS
	return true;
	#else
	return vps_x==0 && vps_y==0 && vps_heading==0;
	#endif
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

unsigned char get_territory(void) {
	unsigned char id = 0;
	float angle = within(-180, 360*atan2(bot.y, bot.x)/(2.0*M_PI), 180);
	printf("%f (%d, %d)\n", angle, bot.x, bot.y);
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
