// Include headers from OS
#include <joyos.h>

// usetup is called during the calibration period. 
int usetup (void) {
    return 0;
}

// Entry point to contestant code.
int umain (void) {
    while (1) {
		motor_set_vel(0, digital_read(0)*frob_read_range(0, 1023));
		motor_set_vel(1, -digital_read(1)*frob_read_range(0, 1023));
	}

    return 0;
}
