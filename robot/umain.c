#include <joyos.h>
#include <math.h>

#include "control.h"
#include "debug.h"
#include "sensors.h"
#include "util_math.h"

#define GYRO_PORT		8
#define LSB_US_PER_DEG	1400000

#define FOSC	8000000
#define BAUD	19200
#define MYUBBR	FOSC/16/BAUD-1

void usart_init(unsigned int ubbr) {
	UBRR1H = (unsigned char)(ubbr>>8);
	UBRR1L = (unsigned char)ubbr;
	//enable receiver and transmitter
	UCSR1B = (1<<RXEN)|(1<<TXEN);
	//set frame format: 8 data, 2 stop
	UCSR1C = (1<<USBS)|(3<<UCSZ0);
}

unsigned char rx1(void) {
	//wait for data
	while(!(UCSR1A&(1<<RXC))) asm volatile ("NOP");
	return UDR1&0x7F;
}

void tx1(unsigned char data) {
	//wait for empty tx buff
	while(!(UCSR1A&(1<<UDRE1))) asm volatile ("NOP");
	//put data in buff
	UDR1 = data;
}

int usetup(void) {
	led_init();
	led_set(0, 1);
	led_set(1, 1);
	led_set(2, 1);

	//set our team # for the VPS
	extern volatile uint8_t robot_id;
	robot_id = 12;

	//initialize the gyro
	gyro_init(GYRO_PORT, LSB_US_PER_DEG, 500L);
	pause(100);
	vps_update();
	gyro_zero();

	//start up the motor controller
	//ctrl_init();

	led_clear();

	return 0;
}

int umain(void) {
	ctrl_init();

	while(1) {
		vps_update();
		move_to(vps_target_x, vps_target_y);
	}

    return 0;
}
