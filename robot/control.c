#include <joyos.h>
#include <math.h>

#include "inc/sensors.h"
#include "inc/control.h"
#include "inc/util_math.h"

#define CURRENT_BLOCKED	15	//indicator of obstruction
#define CURRENT_MAX		20	//danger level for motors
#define TICKS_PER_VPS	1	//TODO conversion between encoders and VPS
#define MAX_SPEED		245	//the fastest the robot will go (leave room for PID)

/* nav settings */
static pid_data pid_linear;

float angle_to_target(int x, int y) {
	return (atan2(y-vps_y, x-vps_x)/M_PI)*180;
}

void move_to(int x, int y) {
    while(true) {
		vps_update();

		//are we there yet?
		if(distance(bot.x, bot.y, x, y)) break;

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

	float integral = 0;
	if(abs(error) > prefs->epsilon)
		integral = integral + error*prefs->dt;
	float derivative = (error - prefs->pre_error)/prefs->dt;
	float output = prefs->Kp*error + prefs->Ki*integral + prefs->Kd*derivative;

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
	pid_linear.epsilon	= 0.1;
	pid_linear.dt		= 0.1;
	pid_linear.Kp		= 3.0;
	pid_linear.Kd		= 0.1;
	pid_linear.Ki		= 0.05;

	//init the settings
	nav_settings.a = 0.5;
	nav_settings.w = 5;

	create_thread(&navigator, STACK_DEFAULT, 0, "nav_thread");
}

void nav_set_heading(float heading) {
	nav_settings.target_heading = heading;
}

void nav_set_velocity(int v) {
	nav_settings.target_velocity = v;
}

void tick_motion(void) {
	//forward acceleration
	if(nav_settings.velocity<nav_settings.target_velocity) {
		nav_settings.velocity += nav_settings.a;
	} else if(nav_settings.velocity>nav_settings.target_velocity) {
		nav_settings.velocity -= nav_settings.a;
	}

	float output = pid_calc(&pid_linear, nav_settings.heading, nav_settings.target_heading);

	#define MAX_TURN	96
	if(output<-MAX_TURN) output = -MAX_TURN;
	if(output>MAX_TURN) output = MAX_TURN;

	//if(abs(bound(-180, bot.heading-nav_settings.heading, 180))<45) {
		motor_set_vel(MOTOR_LEFT, bound(-255, nav_settings.velocity - output, 255));
		motor_set_vel(MOTOR_RIGHT, bound(-255, nav_settings.velocity + output, 255));
	/*} else {
		motor_set_vel(MOTOR_LEFT, output);
		motor_set_vel(MOTOR_RIGHT, -output);
	}*/
}

void tick_state(void) {
	//heading
	nav_settings.heading = gyro_get_degrees();

	//position
	if(vps_is_shit()) {
		int ticks = encoder_read(ENCODER_CENTER);
		float d = convert_encoder(ticks);

		bot.x += d*cos(bot.heading);
		bot.y += d*sin(bot.heading);
	} else {
		bot.x = vps_x;
		bot.y = vps_y;
	}

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

		//printf("[%f, %f]\n", nav_settings.target_heading, nav_settings.heading);

		yield();
	}

	return 0;
}

float convert_encoder(int ticks) {
	return ticks/TICKS_PER_VPS;
}
