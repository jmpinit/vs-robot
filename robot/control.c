#include <joyos.h>
#include <math.h>

#include "inc/debug.h"
#include "inc/sensors.h"
#include "inc/control.h"
#include "inc/util_math.h"

#define CURRENT_BLOCKED	15		//indicator of obstruction
#define CURRENT_MAX		20		//danger level for motors
#define MAX_SPEED		245		//the fastest the robot will go (leave room for PID)
#define ACCEL_ENCODER	10.0

#define GATE_OPEN		110
#define GATE_CLOSED		511
#define LEVER_UP		440
#define LEVER_MIDDLE	245
#define LEVER_DOWN		170

static pid_data pid_linear;

float angle_to_target(int x, int y) {
	return (atan2(y-bot.y, x-bot.x)/M_PI)*180;
}

void move_to_ptp(int x, int y, int vel) {
	do {
		vps_update();
		while(vps_is_shit()) { vps_update(); }
		gyro_zero();

		float dist = vps_to_encoder(distance(bot.x, bot.x, x, y));
		nav_turn_to(angle_to_target(x, y));
		nav_straight_stop(dist, vel);
	} while(distance(bot.x, bot.y, x, y)>256);
}

void move_to(int x, int y) {
	vps_update();
	while(vps_is_shit()) { vps_update(); }
	gyro_zero();

	float dist = vps_to_encoder(distance(bot.x, bot.y, x, y));
	nav_straight(dist, 96);
	bot.target_heading = angle_to_target(x, y);
}

float pid_calc(pid_data* prefs, float current, float target) {
	float error = within(-180, target - current, 180);

	if(abs(error) > prefs->epsilon)
		prefs->integral = prefs->integral + error*prefs->dt;
	float derivative = (error - prefs->pre_error)/prefs->dt;
	float output = prefs->Kp*error + prefs->Ki*prefs->integral + prefs->Kd*derivative;

	//Update error
	prefs->pre_error = error;

	return output;
}

/*
   Motor control abstraction layer
*/

robot bot;

void nav_init(void) {
	//init pid settings
	pid_linear.epsilon	= 0.01;
	pid_linear.dt		= 0.1;
	pid_linear.Kp		= 3.0;
	pid_linear.Kd		= 0.1;
	//pid_linear.Ki		= 0.05;
	pid_linear.Ki		= 0.014;

	//init the settings
	bot.accel = 2;
	bot.deccel = 1;
	bot.w = 5;

	create_thread(&navigator, STACK_DEFAULT, 0, "nav_thread");
}

void nav_set_heading(float heading) {
	bot.target_heading = heading;
}

void nav_set_velocity(int v) {
	bot.target_velocity = v;
}

void nav_straight_stop(int distance, int v) {
	int stop_distance = (float)(distance-(1.0/bot.deccel)*ACCEL_ENCODER);	//account for decceleration
	
	nav_straight(stop_distance, v);
	nav_set_velocity(64);
	while(encoder_read_avg()<distance) { NOTHING; yield(); }	//drive for that length
	nav_set_velocity(0);
	motor_brake(MOTOR_LEFT);
	motor_brake(MOTOR_RIGHT);
	bot.velocity = 0;
}

void nav_straight(int distance, int v) {
	nav_set_heading(bot.heading);	//drive in direction we are facing

	nav_set_velocity(v);
	int ticks_start = encoder_read_avg();
	while(encoder_read_avg()-ticks_start<distance) { NOTHING; yield(); }	//drive for that length
}

void nav_turn_to(float heading) {
	nav_set_velocity(0);
	while(abs(bot.velocity)>bot.accel) { NOTHING; yield(); }	//wait until stopped
	nav_set_heading(heading);
	while(abs(within(-180, bot.heading-heading, 180))>4) { NOTHING; }	//wait until we face that direction
}

void tick_motion(void) {
	//forward acceleration
	if(bot.velocity<bot.target_velocity) {
		bot.velocity += bot.accel;
	} else if(bot.velocity>bot.target_velocity) {
		bot.velocity -= bot.deccel;
	}
	if((bot.velocity>=bot.target_velocity-max(bot.accel, bot.deccel))
		   	&& (bot.velocity<=bot.target_velocity+max(bot.accel, bot.deccel)))
		bot.velocity = bot.target_velocity;

	float output = pid_calc(&pid_linear, bot.heading, bot.target_heading);

	#define MAX_TURN	96
	if(output<-MAX_TURN) output = -MAX_TURN;
	if(output>MAX_TURN) output = MAX_TURN;

	motor_set_vel(MOTOR_LEFT, bound(-255, bot.velocity - output, 255));
	motor_set_vel(MOTOR_RIGHT, bound(-255, bot.velocity + output, 255));
}

int navigator(void) {
	for(;;) {
		tick_motion();

		yield();
	}

	return 0;
}

void gate_open(void) {
	servo_set_pos(SERVO_GATE, GATE_OPEN);
}

void gate_close(void) {
	servo_set_pos(SERVO_GATE, GATE_CLOSED);
}

void lever_up(void) {
	servo_set_pos(SERVO_LEVER, LEVER_UP);
}

void lever_middle(void) {
	servo_set_pos(SERVO_LEVER, LEVER_MIDDLE);
}

void lever_down(void) {
	servo_set_pos(SERVO_LEVER, LEVER_DOWN);
}

