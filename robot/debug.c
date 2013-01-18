#include <avr/interrupt.h>
#include <joyos.h>
#include "debug.h"
#include "control.h"
#include "sensors.h"
#include "term.h"

/*
   DEBUG MAGIC
*/

variable* watchees[NUM_WATCHED];
unsigned char dbg_watch_count = 0;
void dbg_watch(variable* watchee) {
	watchees[dbg_watch_count] = watchee;
	dbg_watch_count++;
}

void dbg_print(unsigned int id) {
	void* address = watchees[id]->address;
	int i;
	float f;

	switch(watchees[id]->type) {
		case INT:
			i = *((int *)address);
			bprintf("%d", i);
			break;
		case FLOAT:
			f = *((float *)address);
			bprintf("%f", f);
			break;
		default:
			bprintf("unrecognized");
	}
}

/*
   BLUETOOTH
*/

FILE* blue_stdio;

void blue_init(unsigned int ubbr) {
	UBRR1H = (unsigned char)(ubbr>>8);
	UBRR1L = (unsigned char)ubbr;
	//enable receiver and transmitter
	UCSR1B = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
	//set frame format: 8 data, 2 stop
	UCSR1C = (1<<USBS)|(3<<UCSZ0);

	blue_stdio = fdevopen(blue_putc, blue_getc);
}

unsigned char blue_rx(void) {
	//wait for data
	while(!(UCSR1A&(1<<RXC))) asm volatile ("NOP");
	return UDR1;
}

int blue_getc(FILE *stream) {
	return (int)blue_rx();;
}

void blue_tx(unsigned char data) {
	//wait for empty tx buff
	while(!(UCSR1A&(1<<UDRE1))) asm volatile ("NOP");
	//put data in buff
	UDR1 = data;
}

int blue_putc(char c, FILE *stream) {
	if(c=='\n') blue_putc('\r', stream);
	blue_tx(c);
	return 0;
}

void blue_print(char *data) {
	char c;
	while((c=*(data++))) blue_tx(c);
}

void bprintf(const char *str, ...) {
	va_list ap;

	va_start(ap, str);         /* Initialize the argument list. */
	vfprintf(blue_stdio, str, ap);
}

unsigned char mode = MODE_WAIT;
ISR(USART1_RX_vect) {
	char data = UDR1;

	switch(mode) {
		case MODE_WAIT:
			switch(data) {
				case ' ':	//KILL
					bprintf("killing...\n");

					//stop everything
					motor_brake(MOTOR_LEFT);
					motor_brake(MOTOR_RIGHT);
					halt();
					break;
				case '!':
					bprintf("==VS ROBOT TERM==\n");
					mode = MODE_TERM;
					term_init();
					break;
				case 'r':
					bprintf("remote control mode. space to escape.\n");
					mode = MODE_CONTROL;
			}
			break;
		case MODE_TERM:
			//TODO ~ exits terminal
			term_consume(data);
			break;
		case MODE_CONTROL:
			switch(data) {
				case 'q':
					motor_set_vel(MOTOR_LEFT, 128);
					break;
				case 'w':
					motor_set_vel(MOTOR_LEFT, 128);
					motor_set_vel(MOTOR_RIGHT, 128);
					break;
				case 'e':
					motor_set_vel(MOTOR_RIGHT, 128);
					break;
				case 'a':
					motor_set_vel(MOTOR_LEFT, -128);
					break;
				case 'd':
					motor_set_vel(MOTOR_RIGHT, -128);
					break;
				case 'z':
					motor_set_vel(MOTOR_LEFT, -128);
					motor_set_vel(MOTOR_RIGHT, -128);
					break;
				case ' ':
					bprintf("waiting for commands...\n");
					mode = MODE_WAIT;
					break;
				default:
					motor_brake(MOTOR_LEFT);
					motor_brake(MOTOR_RIGHT);
			}
			break;
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
