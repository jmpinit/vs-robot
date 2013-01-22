#include <joyos.h>
#include <math.h>

#include "inc/debug.h"
#include "inc/sensors.h"
#include "inc/control.h"
#include "inc/util_math.h"

#define CURRENT_BLOCKED	15		//indicator of obstruction
#define CURRENT_MAX		20		//danger level for motors
#define TICKS_PER_VPS	.07		//conversion between encoders and VPS
#define MAX_SPEED		245		//the fastest the robot will go (leave room for PID)

#define GATE_OPEN		330
#define GATE_CLOSED		80
#define LEVER_UP		235
#define LEVER_MIDDLE	177
#define LEVER_DOWN		112

static pid_data pid_linear;

float angle_to_target(int x, int y) {
	return (atan2(y-vps_y, x-vps_x)/M_PI)*180;
}

void move_to_ptp(int x, int y) {
	vps_update();
	while(vps_is_shit()) { vps_update(); }
	gyro_zero();

	float dist = vps_to_encoder(distance(vps_x, vps_y, x, y));
	nav_turn_to(angle_to_target(x, y));
	nav_straight(dist, 235);
	nav_set_velocity(0);
}

void move_to(int x, int y) {
    while(true) {
		vps_update();

		//are we there yet?
		if(distance(bot.x, bot.y, x, y)<512) break;

		//move towards the target
		nav_set_heading(angle_to_target(x, y));
		nav_set_velocity(frob_read_range(0, MAX_SPEED));

		//correct course
		while(vps_is_shit()) asm volatile("NOP;");
		gyro_zero();
		nav_set_heading(angle_to_target(x, y));

		yield();
	}
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
	pid_linear.Ki		= 0.012;

	//init the settings
	bot.a = 2;
	bot.w = 5;

	create_thread(&navigator, STACK_DEFAULT, 0, "nav_thread");
}

void nav_set_heading(float heading) {
	bot.target_heading = heading;
}

void nav_set_velocity(int v) {
	bot.target_velocity = v;
}

void nav_straight(int distance, int v) {
	nav_set_heading(bot.heading);	//drive in direction we are facing

	nav_set_velocity(v);
	encoder_reset(ENCODER_LEFT);
	encoder_reset(ENCODER_RIGHT);
	while(encoder_read_avg()<distance) { NOTHING; yield(); }	//drive for that length
}

void nav_turn_to(float heading) {
	nav_set_velocity(0);
	while(abs(bot.velocity)>bot.a) { NOTHING; yield(); }	//wait until stopped
	nav_set_heading(heading);
	while(abs(within(-180, bot.heading-heading, 180))>4) { NOTHING; }	//wait until we face that direction
}

void tick_motion(void) {
	//forward acceleration
	if(bot.velocity<bot.target_velocity) {
		bot.velocity += bot.a;
	} else if(bot.velocity>bot.target_velocity) {
		bot.velocity -= bot.a;
	}
	if((bot.velocity>=bot.target_velocity-bot.a)
		   	&& (bot.velocity<=bot.target_velocity+bot.a))
		bot.velocity = bot.target_velocity;

	float output = pid_calc(&pid_linear, bot.heading, bot.target_heading);

	#define MAX_TURN	96
	if(output<-MAX_TURN) output = -MAX_TURN;
	if(output>MAX_TURN) output = MAX_TURN;

	motor_set_vel(MOTOR_LEFT, bound(-255, bot.velocity - output, 255));
	motor_set_vel(MOTOR_RIGHT, bound(-255, bot.velocity + output, 255));
}

void tick_state(void) {
	//heading
	bot.heading = gyro_absolute();

	//position
	/*if(vps_is_shit()) {
		int ticks = encoder_read_avg();
		float d = encoder_convert(ticks);

		bot.x += d*cos(bot.heading);
		bot.y += d*sin(bot.heading);
	} else {
		bot.x = vps_x;
		bot.y = vps_y;
	}*/

	//obstruction
	//printf("[%d, %d]", motor_get_current(0), motor_get_current(1));

	/*bool obstructed = false;
	for(unsigned char i=0; i<4; i++) {
		if(motor_get_current(i)>CURRENT_BLOCKED) obstructed = true;
		//shut off burning motors
		if(motor_get_current(i)>CURRENT_MAX) motor_set_vel(i, 0);
	}

	bot.obstructed = obstructed;*/
}

int navigator(void) {
	for(;;) {
		tick_state();
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

float encoder_read_avg(void) {
	return (encoder_read(ENCODER_LEFT)+encoder_read(ENCODER_RIGHT))/2.0;
}

float vps_to_encoder(float dist) {
	return dist*TICKS_PER_VPS;
}
