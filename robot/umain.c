#include <joyos.h>

#define GYRO_PORT		8
#define LSB_US_PER_DEG	1400000

void led_set(unsigned char led, unsigned char state) {
	if(led<3) {
		if(state)
			PORTD |= 1<<(led+2);
		else
			PORTD &= ~(1<<(led+2));
	}
}

int usetup (void) {
	//create outputs for LEDs
	DDRD = (1<<PD2)|(1<<PD3)|(1<<PD4);

    return 0;
}

int umain (void) {
    while (1) {
		for(int i=0; i<3; i++) {
			led_set(i, 1);
			pause(100);
			led_set(i, 0);
			pause(100);
		}
	}

    return 0;
}
