#include <avr/pgmspace.h>
#include <joyos.h>
#include "term.h"
#include "debug.h"

#define NUM_CMDS 3

const char TICKLE[] PROGMEM = "tickle";
const char POKE[] PROGMEM = "poke";
const char EXIT[] PROGMEM = "exit";

const char *cmds[] PROGMEM = {
	TICKLE,
	POKE,
	EXIT
};

void term_process(void);

char buff[TERM_LEN];
unsigned int pos;
void term_init(void) {
	//fill buff with spaces
	for(unsigned int i=0; i<TERM_LEN; i++)
		buff[i] = ' ';

	pos = 0;

	blue_print("\n\r>");
}

void term_consume(char c) {
	if(c=='\r') {
		term_process();
		term_init();
	} else {
		if(c==8 || c==127) {	//backspace / delete
			if(pos>0) {
				//remove from buff
				pos--;
				buff[pos] = ' ';

				//delete on their terminal
				blue_print("\b \b");
			}
		} else {
			if(pos<TERM_LEN) {
				//store in buff
				buff[pos] = c;
				pos++;

				//echo on their terminal
				blue_tx(c);
			}
		}
	}
}

/* check if string b starts with string a from program space */
bool cmp(unsigned char cmd, char *b) {
	unsigned int addr = 0;
	char c1, c2;
	while((c1=pgm_read_byte(pgm_read_word(&cmds[cmd])+addr++))) {
		c2 = *b++;
		if(c1!=c2) return false;
	}

	return true;
}

void term_process(void) {
	//TODO exec commands
	unsigned char cmd_i = 255;
	for(unsigned char i=0; i<NUM_CMDS; i++) {
		if(cmp(i, buff)) cmd_i = i;
	}
	
	switch(cmd_i) {
		case 0:
			blue_print("\n\rhehehehe");
			break;
		case 1:
			blue_print("\n\rouch! stop that.");
			break;
		case 2:
			blue_print("\n\ri can't do that dave.");
			break;
		default:
			blue_print("\n\ri am my own master");
	}
}
