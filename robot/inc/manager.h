#ifndef _MANAGER_
#define _MANAGER_

#define TEAM_RED	0
#define TEAM_BLUE	1

#include "util_math.h"

typedef struct {
	pt center;
	pt mine;
	pt capture;
	float heading_mine;
	float heading_capture;
} territory;

extern unsigned char team;

/*enum Activity {
	CAPTURE,
	DUMP,
	EXPLORE,
	MINE
};*/

//pick the next 
//activity act_pick(void);
void manager_explore(void);
void manager_visit(void);

#endif
