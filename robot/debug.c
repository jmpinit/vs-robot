#include <joyos.h>
#include "debug.h"
#include "sensors.h"

static int ledstate = 0;
int led_tick(void) {
	for(;;){
		led_clear();

		/*if(vps_is_shit()) {
			if(ledstate==0) {
				led_set(0, false);
				led_set(1, false);
				led_set(2, false);
				
				ledstate = 1;
			} else {
				led_set(0, true);
				led_set(1, true);
				led_set(2, true);

				ledstate = 0;
			}
		} else {*/
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
		//}

		ledstate++;

		yield(); //tell joyos this thread is done for now
	}

	return 0;
}

void led_start(void) {
	create_thread(&led_tick, STACK_DEFAULT, 0, "led_thread");
}

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
