#include <joyos.h>
#include <math.h>

#include "control.h"
#include "debug.h"
#include "sensors.h"
#include "util_math.h"

#define GYRO_PORT		8
#define LSB_US_PER_DEG	1400000

int usetup(void) {
	blue_init(MYUBBR);

	/*led_init();
	led_set(0, 1);
	led_set(1, 1);
	led_set(2, 1);*/

	//set our team # for the VPS
	extern volatile uint8_t robot_id;
	robot_id = 12;

	//initialize the gyro
	gyro_init(GYRO_PORT, LSB_US_PER_DEG, 500L);
	pause(100);
	vps_update();
	gyro_zero();

	//led_clear();

	return 0;
}

int umain(void) {
	while(true) {
		asm volatile ("NOP");
	}

	ctrl_init();

	while(1) {
		vps_update();
		move_to(vps_target_x, vps_target_y);
	}

    return 0;
}
