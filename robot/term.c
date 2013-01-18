#include <avr/pgmspace.h>
#include <joyos.h>
#include "term.h"
#include "debug.h"

#define BUFF_LEN	64
#define NUM_CMDS	3

/* command info */
const char TOK_SET[] PROGMEM = "set";		//set <# of watched var> <int value>
const char TOK_VIEW[] PROGMEM = "view";		//view <# of watched var>
const char TOK_FOLLOW[] PROGMEM = "follow";	//continuously watch

const char *cmds[] PROGMEM = {
	TOK_SET,
	TOK_VIEW,
	TOK_FOLLOW
};

enum CMD {
	SET,
	VIEW,
	FOLLOW
};

void term_process(void);

char buff[BUFF_LEN];
unsigned int pos;
void term_init(void) {
	//fill buff with spaces
	for(unsigned int i=0; i<BUFF_LEN; i++)
		buff[i] = ' ';

	pos = 0;

	bprintf(">");
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
				bprintf("\b \b");
			}
		} else {
			if(pos<BUFF_LEN) {
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

/*
unsigned int get_param(unsigned char num) {
	//find the start of it
	for(unsigned char i=0; i<num; i++) {
		unsigned char pos = 0;
		while(buff[pos]!=' ') pos++;
		pos++;
	}

	//return failure if that parameter doesn't exist
	if(pos>=BUFF_LEN) return -1;

	//count the digits in the number
	unsigned char end = pos;
	while(buff[end]>=48 && buff[end]<=57) end++;
	unsigned char length = end-pos;

	//calculate the value of the number
	unsigned char total = 0;
	for(unsigned char digit=length-1; digit>=0; digit--) {
		unsigned char val = buff[pos] - 48; //get the value
		pos++;

		unsigned char tens = 1;
		for(int t=0; t<digit; t++) tens *= 10;
		total += val*tens;
	}

	return total;
}*/

unsigned int dbg_index;
int update(void) {
	while(true) {
		dbg_print(dbg_index);
		bprintf("\n");
		yield();
	}

	return 0;
}

void term_process(void) {
	//TODO exec commands
	enum CMD cmd_i = 255;
	for(unsigned char i=0; i<NUM_CMDS; i++) {
		if(cmp(i, buff)) cmd_i = i;
	}
	
	switch(cmd_i) {
		case SET:
			break;
		case VIEW:
			bprintf("\n");
			bprintf("tracking %d variables.\n", dbg_watch_count);
			if(dbg_watch_count==0) {
				bprintf("none to view.\n");
			} else {
				bprintf("enter # to view: ");
				unsigned int index;
				fscanf(blue_stdio, "%d", &index);
				bprintf("\nvalue is ");
				dbg_print(0);
			}
			break;
		case FOLLOW:
			bprintf("enter # to view: ");
			fscanf(blue_stdio, "%d", &dbg_index);
			bprintf("\n");
			create_thread(&update, STACK_DEFAULT, 1, "follower_thread");
			break;
		default:
			bprintf("not understood.\n");
	}
}
