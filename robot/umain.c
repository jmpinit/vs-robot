// Include headers from OS
#include <joyos.h>

// usetup is called during the calibration period. 
int usetup (void) {
    return 0;
}

// Entry point to contestant code.
int umain (void) {
    while (1) {
		if(digital_read(0)) {
			motor_set_vel(0, 255);
			printf("switch triggered\n");
			pause(2000);
			motor_set_vel(0, 0);
		}
	}

    return 0;
}
