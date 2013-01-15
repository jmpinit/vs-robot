#include <joyos.h>
#include <math.h>

#include "sensors.h"
#include "control.h"
#include "util_math.h"

#define DIST_CHECK	128
#define DIST_CLOSE	512
#define MAX_SPEED	245	//the fastest the robot will go
#define MIN_SPEED	96	//speed of approach

//pid settings for moving straight
pid_data pid_linear_settings = {
	0.1,
	0.1,
	3.0,
	0.1,
	0.05,
	0
};

#define DIST_HIST	4
int distances[DIST_HIST];
int avg_distance(void) {
	float total = 0;
	for(unsigned char i=0; i<DIST_HIST; i++)
		total += distances[i];
	return total/((float)(DIST_HIST));
}

void tick_distance(int d) {
	//move back history
	for(unsigned char i=DIST_HIST-1; i>0; i--)
		distances[i] = distances[i-1];
	distances[0] = d;
}

float angle_to_target(int x, int y) {
	return (atan2(y-vps_y, x-vps_x)/M_PI)*180;
}

void move_to(int x, int y) {
	float current_dist;
	float last_distance = 0;

    do {
		vps_update();

		//keep track of distance to target
		if(!vps_is_shit())
			current_dist = distance(vps_x, vps_y, x, y);
		else
			current_dist = last_distance;

		tick_distance(current_dist);
		last_distance = avg_distance();

		//move towards the target
		ctrl_set_heading(angle_to_target(x, y));
		ctrl_set_speed(MIN_SPEED + within(0, frob_read_range(0, MAX_SPEED)-MIN_SPEED, MAX_SPEED)*current_dist/DIST_CLOSE);

		//correct course
		if(encoder_read(MOTOR_LEFT) % DIST_CHECK == 0) {
			while(vps_is_shit()) asm volatile("NOP;");
			gyro_zero();
			ctrl_set_heading(angle_to_target(x, y));
		}

		yield();
	} while(current_dist>128);

	printf("done moving to (%d, %d)\n", x, y);
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

ctrl_data ctrl_settings;
void ctrl_init(void) {
	create_thread(&motor_controller, STACK_DEFAULT, 0, "ctrl_thread");

	//init the settings
	ctrl_settings.speed = 0;
	ctrl_settings.target = 128;
	ctrl_settings.heading = 0;
	ctrl_settings.slope = 5;
}

void ctrl_set_heading(float heading) {
	ctrl_settings.heading = heading;
}

void ctrl_set_speed(int speed) {
	ctrl_settings.target = speed;
}

int motor_controller(void) {
	for(;;) {
		//printf("target=%d, speed=%f\n", ctrl_settings.target, ctrl_settings.speed);

		if(ctrl_settings.speed<ctrl_settings.target-ctrl_settings.slope) {
			ctrl_settings.speed += ctrl_settings.slope;
		} else if(ctrl_settings.speed>ctrl_settings.target+ctrl_settings.slope) {
			ctrl_settings.speed -= ctrl_settings.slope;
		} else {
			ctrl_settings.speed = ctrl_settings.target;
		}

		float heading;
		if(vps_is_shit())
			heading = gyro_get_degrees();
		else
			heading = gyro_absolute();

		float output = pid_calc(pid_linear_settings, heading, ctrl_settings.heading);

		if(abs(bound(-180, heading-ctrl_settings.heading, 180))<60) {
			motor_set_vel(MOTOR_LEFT, ctrl_settings.speed + output);
			motor_set_vel(MOTOR_RIGHT, ctrl_settings.speed - output);
		} else {
			motor_set_vel(MOTOR_LEFT, output);
			motor_set_vel(MOTOR_RIGHT, -output);
		}

		yield();
	}

	return 0;
}
