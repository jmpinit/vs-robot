#include <avr/interrupt.h>
#include <joyos.h>
#include "debug.h"
#include "control.h"

/*
   BLUETOOTH
*/

void blue_init(unsigned int ubbr) {
	UBRR1H = (unsigned char)(ubbr>>8);
	UBRR1L = (unsigned char)ubbr;
	//enable receiver and transmitter
	UCSR1B = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
	//set frame format: 8 data, 2 stop
	UCSR1C = (1<<USBS)|(3<<UCSZ0);
	
}

unsigned char blue_rx(void) {
	//wait for data
	while(!(UCSR1A&(1<<RXC))) asm volatile ("NOP");
	return UDR1;
}

void blue_tx(unsigned char data) {
	//wait for empty tx buff
	while(!(UCSR1A&(1<<UDRE1))) asm volatile ("NOP");
	//put data in buff
	UDR1 = data;
}

int speed = 0;
ISR(USART1_RX_vect) {
	char result = UDR1;
	switch(result) {
		case 'q':
			motor_set_vel(MOTOR_LEFT, speed);
			break;
		case 'w':
			motor_set_vel(MOTOR_LEFT, speed);
			motor_set_vel(MOTOR_RIGHT, speed);
			break;
		case 'e':
			motor_set_vel(MOTOR_RIGHT, speed);
			break;
		case 'a':
			motor_set_vel(MOTOR_LEFT, -speed);
			break;
		case 'd':
			motor_set_vel(MOTOR_RIGHT, -speed);
			break;
		case 'z':
			motor_set_vel(MOTOR_LEFT, -speed);
			motor_set_vel(MOTOR_RIGHT, -speed);
			break;
		case '0':
			speed = 64;
			break;
		case '1':
			speed = 128;
			break;
		case '2':
			speed = 256;
			break;
		default:
			motor_brake(MOTOR_LEFT);
			motor_brake(MOTOR_RIGHT);
	}
}

/*
   LED CONTROL
   */

static int ledstate = 0;
static int ledtimer = 0;
int led_tick(void) {
	for(;;){
		if(ledtimer>1000) {
			led_clear();

			if(vps_is_shit()) {
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
			} else {
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
			}


			ledstate++;
			ledtimer = 0;
		} else {
			ledtimer++;
		}

		yield(); //tell joyos this thread is done for now
	}

	return 0;
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

	create_thread(&led_tick, STACK_DEFAULT, 0, "led_thread");
}
