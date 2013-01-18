#include <avr/pgmspace.h>
#include <joyos.h>
#include "inc/term.h"
#include "inc/debug.h"

#define BUFF_LEN	64

//determines where input should go
enum MODE {
	CMD,
	FOLLOWING
} mode;

/* command info */
//TODO add fset command (set a floating point value)
const char TOK_HELP[] PROGMEM	= "help";		//displays list of commands
const char TOK_SET[] PROGMEM	= "set";		//set <# of watched var> <int value>
const char TOK_VIEW[] PROGMEM	= "view";		//view <# of watched var>
const char TOK_ALL[] PROGMEM	= "all";		//see state of all watched variables
const char TOK_FOLLOW[] PROGMEM	= "follow";		//continuously watch
const char TOK_MOTOR[] PROGMEM	= "motor";		//control motor

const char *cmds[] PROGMEM = {
	TOK_HELP,
	TOK_SET,
	TOK_VIEW,
	TOK_ALL,
	TOK_FOLLOW,
	TOK_MOTOR
};

enum CMD {
	HELP,
	SET,
	VIEW,
	ALL,
	FOLLOW,
	MOTOR
};

/* private functions */
void clear_buff(void);
void term_process(void);

/* command variables */
unsigned int dbg_index;

/* terminal state */
char buff[BUFF_LEN];
unsigned int pos;

void clear_buff(void) {
	//fill buff with spaces
	for(unsigned int i=0; i<BUFF_LEN; i++)
		buff[i] = ' ';
	pos = 0;
}

void term_init(void) {
	clear_buff();
	mode = CMD;

	bprintf(">");
}

void term_consume(char c) {
	if(c=='\r') {
		if(mode==CMD) {
			bprintf("\n");
			term_process();
			if(mode==CMD) bprintf("\n>");
			clear_buff();
		} else if(mode==FOLLOWING) {
			mode = CMD;
			bprintf("\n");
			term_init();
		}
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

int update(void) {
	while(mode==FOLLOWING) {
		dbg_print(dbg_index);
		bprintf("\n");
		pause(100);
	}

	return 0;
}

void term_process(void) {
	//TODO exec commands
	enum CMD cmd_i = 255;
	for(unsigned char i=0; i<ARRAY_SIZE(cmds); i++) {
		if(cmp(i, buff)) cmd_i = i;
	}
	
	unsigned int a, b;
	float fa, fb;
	switch(cmd_i) {
		case HELP:
			bprintf("< commands >\n");
			bprintf("set\t- edit value\n"
					"all\t- view all\n"
					"view\t- view one\n"
					"follow\t- view changes\n");
			break;
		case SET:
			sscanf(buff, "%*s %d %d", &a, &b);
			dbg_set(a, &b, INT);
			break;
		case ALL:
			bprintf("| %d vars\t|\n", dbg_watch_count);
			for(unsigned char i=0; i<dbg_watch_count; i++) {
				bprintf("%d:\t", i);
				dbg_print(i);
				bprintf("\n");
			}
			break;
		case VIEW:
			sscanf(buff, "%*s %d", &a);
			bprintf("val is ");
			dbg_print(a);
			break;
		case FOLLOW:
			sscanf(buff, "%*s %d %d", &a);
			mode = FOLLOWING;
			create_thread(&update, STACK_DEFAULT, 1, "follower_thread");
			break;
		case MOTOR:
			sscanf(buff, "%*s %d %d", &a, &b);
			motor_set_vel(a, b);
			break;
		default:
			bprintf("not understood.\n");
	}
}
