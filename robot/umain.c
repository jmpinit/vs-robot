#include <joyos.h>
#include <math.h>

#include "control.h"
#include "debug.h"
#include "sensors.h"
#include "util_math.h"

#define GYRO_PORT		8
#define LSB_US_PER_DEG	1400000

int usetup(void) {
	led_init();
	led_set(0, 1);
	led_set(1, 1);
	led_set(2, 1);

	//set our team # for the VPS
	extern volatile uint8_t robot_id;
	robot_id = 12;

	//initialize the gyro
	gyro_init(GYRO_PORT, LSB_US_PER_DEG, 500L);
	pause(100);
	gyro_zero();

	led_clear();

	return 0;
}

int umain (void) {
	led_set(0, true);
	pause(100);
	led_set(1, true);	//about to start!
	pause(100);
	led_set(2, true);	//we're on our way
	pause(100);

    while(1) {
		//movement test
		while(1) {
			printf("moving to first pt.");
			vps_update();
			move_to(vps_target_x, vps_target_y);
			printf("i think i am there.");
			motor_set_vel(0, 0);
			motor_set_vel(1, 0);
			pause(1000);
		}
	}

    return 0;
}
