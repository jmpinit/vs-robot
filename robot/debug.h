#ifndef _DEBUG_
#define _DEBUG_

#define NUM_WATCHED	16	//number of variables that can be watched at one time

#define FOSC	8000000
#define BAUD	19200
#define MYUBBR	FOSC/16/BAUD-1

#define MODE_WAIT		0
#define MODE_TERM		1
#define MODE_CONTROL	2

extern FILE* blue_stdio;

extern unsigned char dbg_watch_count;

enum TYPE {
	INT,
	FLOAT
};

typedef struct {
	enum TYPE type;
	void* address;
} variable; 

void dbg_watch(variable* watchee);	//start watching a variable
void dbg_print(unsigned int id);	//print value of watchee over BT

void blue_init(unsigned int ubbr);	//config USART1 for BT module and setup RX interrupt
unsigned char blue_rx(void);		//wait for byte over BT and read it
int blue_getc(FILE *stream);
void blue_tx(unsigned char data);	//transmit byte over BT
int blue_putc(char c, FILE *stream);
void blue_print(char *data);
void bprintf(const char *str, ...);

void led_init(void);
void led_clear(void);
void led_set(unsigned char led, bool state);

#endif
