#ifndef _UTIL_DEBUG_
#define _UTIL_DEBUG_

#define FOSC	8000000
#define BAUD	19200
#define MYUBBR	FOSC/16/BAUD-1

void blue_init(unsigned int ubbr);	//config USART1 for BT module and setup RX interrupt
void blue_tx(unsigned char data);	//transmit byte over BT
unsigned char blue_rx(void);		//wait for byte over BT and read it

void led_init(void);
void led_clear(void);
void led_set(unsigned char led, bool state);

#endif
