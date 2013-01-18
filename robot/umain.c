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

	float gyro_pos;
	int encoder_val;
	dbg_watch(&gyro_pos, FLOAT);
	dbg_watch(&encoder_val, INT);

	while(true) {
		gyro_pos = gyro_get_degrees();
		encoder_val = encoder_read(24);
		//TODO high-level programming
	}

    return 0;
}
