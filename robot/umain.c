#include <joyos.h>

#define GYRO_PORT		8
#define LSB_US_PER_DEG	1400000

#define MOTOR_LEFT	0
#define MOTOR_RIGHT	1

void led_init(void) {
	//create outputs for LEDs
	DDRD = (1<<PD2)|(1<<PD3)|(1<<PD4);
}

void led_set(unsigned char led, unsigned char state) {
	if(led<3) {
		if(state)
			PORTD |= 1<<(led+2);
		else
			PORTD &= ~(1<<(led+2));
	}
}

int usetup(void) {
	led_init();

	printf("\nPlace robot,    press go.");
	go_click ();
	printf ("\nStabilizing...");
	pause (500);
	printf ("\nCalibrating     offset...\n");
	gyro_init (GYRO_PORT, LSB_US_PER_DEG, 500L);

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

int umain (void) {
	float desired = gyro_get_degrees();

    while(1) {
		int motor_bias = frob_read_range(0, 255);
		//motor_set_vel(0, frob_read_range(0, 255)-128);
		//motor_set_vel(1, frob_read_range(0, 255)-128);
		float heading = gyro_get_degrees();
		float error = bound(desired - heading, 360.0);

		printf("error is %d\n", error);

		motor_set_vel(MOTOR_LEFT, within(0, motor_bias + error, 255));
		motor_set_vel(MOTOR_RIGHT, within(0, motor_bias - error, 255));
	}

    return 0;
}
