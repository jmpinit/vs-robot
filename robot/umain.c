#include <joyos.h>

#define GYRO_PORT		8
#define LSB_US_PER_DEG	1400000

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
    return 0;
}

int umain (void) {
    while(1) {
	}

    return 0;
}
