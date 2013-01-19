#include <joyos.h>
#include <math.h>

#include "inc/control.h"
#include "inc/debug.h"
#include "inc/sensors.h"
#include "inc/util_math.h"

#define GYRO_PORT		8
#define LSB_US_PER_DEG	1400000

int usetup(void) {
	blue_init(MYUBBR);

	//set our team # for the VPS
	extern volatile uint8_t robot_id;
	robot_id = 12;

	//initialize the gyro
	gyro_init(GYRO_PORT, LSB_US_PER_DEG, 500L);
	pause(100);

	return 0;
}

int umain(void) {
	nav_init();

	nav_set_velocity(128);
	while(true) {
		//drive in a square
		nav_set_heading(0);
		pause(4000);
		nav_set_heading(90);
		pause(4000);
		nav_set_heading(178);
		pause(4000);
		nav_set_heading(-90);
		pause(4000);
	}
    return 0;
}
