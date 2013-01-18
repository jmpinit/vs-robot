#include <joyos.h>
#include <math.h>

#include "inc/sensors.h"
#include "inc/control.h"
#include "inc/util_math.h"

#define MAX_SPEED	245	//the fastest the robot will go (leave room for PID)

/* nav settings */
static pid_data pid_linear;
static nav_data nav_settings;

float angle_to_target(int x, int y) {
	return (atan2(y-vps_y, x-vps_x)/M_PI)*180;
}

void move_to(int x, int y) {
    while(true) {
		vps_update();

		//are we there yet?
		

		//move towards the target
		nav_set_heading(angle_to_target(x, y));
		nav_set_speed(frob_read_range(0, MAX_SPEED));

		//correct course
		while(vps_is_shit()) asm volatile("NOP;");
		gyro_zero();
		nav_set_heading(angle_to_target(x, y));

		yield();
	}
}

float pid_calc(pid_data prefs, float current, float target) {
	float error = bound(-180, target - current, 180);

	float integral = 0;
	if(abs(error) > prefs.epsilon)
		integral = integral + error*prefs.dt;
	float derivative = (error - prefs.pre_error)/prefs.dt;
	float output = prefs.Kp*error + prefs.Ki*integral + prefs.Kd*derivative;

	//Update error
	prefs.pre_error = error;

	return output;
}

/*
   Motor control abstraction layer
*/

void nav_init(void) {
	create_thread(&motor_controller, STACK_DEFAULT, 0, "nav_thread");

	//init pid settings
	pid_linear.epsilon	= 0.1;
	pid_linear.dt		= 0.1;
	pid_linear.Kp		= 3.0;
	pid_linear.Kd		= 0.1;
	pid_linear.Ki		= 0.05;

	//init the settings
	nav_settings.speed = 0;
	nav_settings.target = 128;
	nav_settings.heading = 0;
	nav_settings.slope = 5;
}

void nav_set_heading(float heading) {
	nav_settings.heading = heading;
}

void nav_set_speed(int speed) {
	nav_settings.target = speed;
	printf("speed changed\n");
}

int motor_controller(void) {
	for(;;) {
		if(nav_settings.speed<nav_settings.target-nav_settings.slope) {
			nav_settings.speed += nav_settings.slope;
		} else if(nav_settings.speed>nav_settings.target+nav_settings.slope) {
			nav_settings.speed -= nav_settings.slope;
		} else {
			nav_settings.speed = nav_settings.target;
		}

		float heading = gyro_absolute();
		float output = pid_calc(pid_linear, heading, nav_settings.heading);

		if(abs(bound(-180, heading-nav_settings.heading, 180))<60) {
			output /= 4.0;
			motor_set_vel(MOTOR_LEFT, nav_settings.speed + output);
			motor_set_vel(MOTOR_RIGHT, nav_settings.speed - output);
		} else {
			motor_set_vel(MOTOR_LEFT, output);
			motor_set_vel(MOTOR_RIGHT, -output);
		}

		yield();
	}

	return 0;
}
