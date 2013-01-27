#ifndef _MANAGER_
#define _MANAGER_

#include "util_math.h"

#define TEAM_RED	0
#define TEAM_BLUE	1

extern unsigned char team;

/*enum Activity {
	CAPTURE,
	DUMP,
	EXPLORE,
	MINE
};*/

/*
   AI
*/

void play(void);

//pick the next 
//activity act_pick(void);
void circle(unsigned int r, int vel);
void visit_one(unsigned char id);

#endif
