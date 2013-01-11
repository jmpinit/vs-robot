#include <joyos.h>

#define GYRO_PORT		8
#define LSB_US_PER_DEG	1400000

int usetup (void) {
	printf("\nPlace robot,    press go.");
	go_click ();
	printf ("\nStabilizing...");
	pause (500);
	printf ("\nCalibrating     offset...\n");
	gyro_init (GYRO_PORT, LSB_US_PER_DEG, 500L);

    return 0;
}

int umain (void) {
    while (1) {
		printf ("\ntheta = %.2f", gyro_get_degrees());
        pause (100);		
	}

    return 0;
}
