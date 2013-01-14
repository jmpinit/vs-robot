#include <joyos.h>
#include "debug.h"

void led_set(unsigned char led, bool state) {
	if(led<3) {
		if(state)
			PORTD |= 1<<(led+2);
		else
			PORTD &= ~(1<<(led+2));
	}
}

void led_clear(void) {
	//turn off the LEDS
	for(unsigned char i=0; i<3; i++) led_set(i, false);
}

void led_init(void) {
	DDRD = (1<<PD2)|(1<<PD3)|(1<<PD4); //create outputs for LEDs
	led_clear();
}
