#include <joyos.h>
#include "pid.h"

#define TRUE	1
#define FALSE	0

#define GYRO_PORT		8
#define LSB_US_PER_DEG	1400000

#define MOTOR_LEFT	0
#define MOTOR_RIGHT	1

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

	led_set(0, TRUE);

	printf("\nPlace robot,    press go.");
	go_click ();
	printf ("\nStabilizing...");
	pause (500);
	printf ("\nCalibrating     offset...\n");
	gyro_init (GYRO_PORT, LSB_US_PER_DEG, 500L);

	led_set(1, TRUE);

	return 0;
}

int within(int min, int val, int max) {
	if(val<min) return min;
	if(val>max) return max;
	return val;
}

float bound(float val, float max) {
	if(val<max) return val;
	
	while(val>max)
		val -= max;

	return val;
}

//TODO put PID stuff in a struct for cleanliness
float pre_error = 0;
int umain (void) {
	pid_data pid_linear_settings = {
		0.1,
		0.1,
		3.0,
		0.5,
		0.1,
		0
	};

	led_set(2, TRUE);
	pause(2000);

	float desired = gyro_get_degrees();

    while(1) {
		//int motor_bias = frob_read_range(0, 255);
		int motor_bias = 255;

		float heading = gyro_get_degrees();
		float output = pid_calc(pid_linear_settings, heading, desired);

		motor_set_vel(MOTOR_LEFT, within(0, motor_bias + output, 255));
		motor_set_vel(MOTOR_RIGHT, within(0, motor_bias - output, 255));

		printf("output=%d", output);
	}

    return 0;
}
