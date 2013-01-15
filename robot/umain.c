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
	go_press();

	gyro_init(GYRO_PORT, LSB_US_PER_DEG, 500L);
	pause(100);
	vps_update();
	gyro_zero();

	led_clear();

	return 0;
}

int ledstate = 0;
int ledtimer = 0;
int led_tick(void) {
	for(;;){
		if(ledtimer>1000) {
			led_clear();

			switch(ledstate%3) {
				case 0:
					led_set(0, true);
					break;
				case 1:
					led_set(1, true);
					break;
				case 2:
					led_set(2, true);
					break;
			}
			ledstate++;
			ledtimer = 0;
		} else {
			ledtimer++;
		}

		yield(); //tell joyos this thread is done for now
	}

	return 0;
}

int umain(void) {
	create_thread(&led_tick, STACK_DEFAULT, 0, "led_thread");
	create_thread(&ramper_thread, STACK_DEFAULT, 0, "ramper_thread");

	#include "calibration/check_ramper.c"

    while(1) {
		while(1) {
			vps_update();
			move_to(vps_target_x, vps_target_y);
			motor_set_vel(0, 0);
			motor_set_vel(1, 0);
		}
	}

    return 0;
}
