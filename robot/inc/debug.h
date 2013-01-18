#ifndef _DEBUG_
#define _DEBUG_

#define ARRAY_SIZE(foo) (sizeof(foo)/sizeof(foo[0]))

#define NUM_WATCHED	16	//number of variables that can be watched at one time

#define FOSC	8000000
#define BAUD	19200
#define MYUBBR	FOSC/16/BAUD-1

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

void dbg_watch(void* addr, enum TYPE type);	//start watching a variable
void dbg_print(unsigned char id);	//print value of watchee over BT
void dbg_set(unsigned char id, void* val, enum TYPE type);
enum TYPE dbg_type(unsigned char id);

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
