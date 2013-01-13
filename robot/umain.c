#include <joyos.h>
#include <math.h>

#include "util_math.h"
#include "pid.h"
#include "util_vps.h"

#define TRUE	1
#define FALSE	0

#define GYRO_PORT		8
#define LSB_US_PER_DEG	1400000

#define MOTOR_LEFT	0
#define MOTOR_RIGHT	1

pid_data pid_linear_settings = {
	0.1,
	0.1,
	3.0,
	0.5,
	0.1,
	0
};

void led_set(unsigned char led, unsigned char state) {
	if(led<3) {
		if(state)
			PORTD |= 1<<(led+2);
		else
			PORTD &= ~(1<<(led+2));
	}
}

void led_init(void) {
	//create outputs for LEDs
	DDRD = (1<<PD2)|(1<<PD3)|(1<<PD4);
	//turn off the LEDS
	for(unsigned char i=0; i<3; i++) led_set(i, FALSE);
}

int usetup(void) {
	led_init();

	//set our team # for the VPS
	extern volatile uint8_t robot_id;
	robot_id = 12;

	gyro_init (GYRO_PORT, LSB_US_PER_DEG, 500L);
	led_set(0, TRUE); //first LED indicates calibration completed

	return 0;
}

//attempts to take robot to VPS coordinate
void move_to(int x, int y) {
	pt loc = vps_pos_us();

	float desired = atan2(y-loc.y, x-loc.x);

	//move in that direction until we are on top of it
    while(dist(loc.x, loc.y, x, y)>16) {
		int motor_bias = 245;

		float heading = vps_angle_current(); //TODO fuse VPS and gyro data
		float output = pid_calc(pid_linear_settings, heading, desired);

		motor_set_vel(MOTOR_LEFT, within(0, motor_bias + output, 255));
		motor_set_vel(MOTOR_RIGHT, within(0, motor_bias - output, 255));
		
		//get our new location
		loc = vps_pos_us();
	}
}

int umain (void) {
	led_set(1, TRUE);	//about to start!
	pause(2000);
	led_set(2, TRUE);	//we're on our way

    while(1) {
		move_to(0, 0);	
	}

    return 0;
}
