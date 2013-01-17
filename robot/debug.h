#ifndef _DEBUG_
#define _DEBUG_

#define FOSC	8000000
#define BAUD	19200
#define MYUBBR	FOSC/16/BAUD-1

#define MODE_WAIT		0
#define MODE_TERM		1
#define MODE_CONTROL	2

void blue_init(unsigned int ubbr);	//config USART1 for BT module and setup RX interrupt
unsigned char blue_rx(void);		//wait for byte over BT and read it
void blue_tx(unsigned char data);	//transmit byte over BT
void blue_print(char *data);

void led_init(void);
void led_clear(void);
void led_set(unsigned char led, bool state);

#endif
