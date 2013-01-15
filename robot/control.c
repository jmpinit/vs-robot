#include <joyos.h>
#include <math.h>

#include "sensors.h"
#include "control.h"
#include "util_math.h"

#define CHECK_DIST	1500.0
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

static int* target_speed = 0;
static float* speed = 0;
int ramper_thread(void) {
	ramper_settings ramper = {0, 0, 0.1};
	speed = &ramper.current;
	target_speed = &ramper.target;

	for(;;) {
		if(ramper.current<ramper.target) {
			ramper.current += ramper.slope;
		} else {
			ramper.current -= ramper.slope;
		}

		yield();
	}

	return 0;
}

void move_to(int x, int y) {
	float desired = 0;
	float current_dist;
	float last_distance = 0;
	bool turning = false;

    do {
		vps_update();

		//printf("(%d, %d)\n", vps_x, vps_y);

		if(!vps_is_shit())
			current_dist = distance(vps_x, vps_y, x, y);
		else
			//TODO use encoders here
			current_dist = last_distance;

		tick_distance(current_dist);
		last_distance = avg_distance();

		float heading = gyro_absolute();

		//set our speed of approach
		if(abs(bound(-180, heading-desired, 180))<60) {
			if(turning) last_distance += 256;
			turning = false;

			#define DIST_CLOSE 1500.0
			if(current_dist>DIST_CLOSE) 
				//far from target -> go fast
				*target_speed = frob_read_range(0, MAX_SPEED);
			else
				//getting close -> slow down but not all the way
				*target_speed = MIN_SPEED + within(0, frob_read_range(0, MAX_SPEED)-MIN_SPEED, MAX_SPEED)*current_dist/DIST_CLOSE;
		} else {
			*target_speed = 0;
			turning = true;
		}

		float output = pid_calc(pid_linear_settings, heading, desired);

		//bound the turning speed
		#define TURN_SPEED		64
		if(turning) {
			if(output<-TURN_SPEED) output = -TURN_SPEED;
			if(output>TURN_SPEED) output = TURN_SPEED;
		}

		motor_set_vel(MOTOR_LEFT, within(-255, *speed + output, 255));
		motor_set_vel(MOTOR_RIGHT, within(-255, *speed - output, 255));

		//correct course
		printf("%f", output);
		if(!turning && abs(output)<6) {
			while(vps_is_shit()) asm volatile("NOP;");
			gyro_zero();
			desired = angle_to_target(x, y);
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
