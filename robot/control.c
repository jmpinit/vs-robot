#include <joyos.h>
#include <math.h>

#include "sensors.h"
#include "control.h"
#include "util_math.h"

#define MAX_SPEED	245	//the fastest the robot will go
#define DIST_CLOSE	512	//where to start looking for target

//pid settings for moving straight
pid_data pid_linear_settings = {
	0.1,
	0.1,
	3.0,
	0.1,
	0.05,
	0
};

void move_to(int x, int y) {
	vps_update();

	//find the direction we need to go (-180 to 180)
	float desired = (atan2(y-vps_y, x-vps_x)/M_PI)*180;
	printf("angle to target is %f degrees", desired);

	float current_dist;
	int old_x = vps_x;
	int old_y = vps_y;

    do {
		vps_update();

		current_dist = distance(vps_x, vps_y, x, y);

		//set our speed of approach
		int motor_bias;
		if(current_dist>512) 
			//far from target -> go fast
			motor_bias = frob_read_range(0, MAX_SPEED);
		else
			//getting close -> slow down but not all the way
			motor_bias = current_dist/(512.0/frob_read_range(0, MAX_SPEED))+32;

		float heading = gyro_absolute(); //TODO fuse VPS and gyro data
		float output = pid_calc(pid_linear_settings, heading, desired);

		motor_set_vel(MOTOR_LEFT, within(0, motor_bias + output, 255));
		motor_set_vel(MOTOR_RIGHT, within(0, motor_bias - output, 255));
		
		//update where we are	
		vps_update();
		if(abs(output)<5.0) { //TODO check if VPS is lying
			printf("updated gyro");
			gyro_transform = gyro_get_degrees()-vps_get_degrees();
		}
		
		int dx = vps_x;
		int dy = vps_y;

		if(abs(output)<5.0) {
			printf("updated pos");
			desired = (atan2(y-(vps_y+dy), x-(vps_x+dx))/M_PI)*180;
		}

		printf("%f\n", output);

		old_x = vps_x;
		old_y = vps_y;

		//get our new location
		vps_update();
		//printf("dist=%f	", distance(vps_x, vps_y, x, y));
		//printf("(%d, %d) @ %f\n", vps_x, vps_y, heading);
	} while(current_dist>128);
}

float pid_calc(pid_data prefs, float current, float target) {
	float error = (target - current);

	float integral = 0;
	if(abs(error) > prefs.epsilon)
		integral = integral + error*prefs.dt;
	float derivative = (error - prefs.pre_error)/prefs.dt;
	float output = prefs.Kp*error + prefs.Ki*integral + prefs.Kd*derivative;

	//Update error
	prefs.pre_error = error;

	return output;
}
