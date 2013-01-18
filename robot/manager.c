#include <joyos.h>
#include "inc/manager.h"

bool team;

/* scoring information */
enum {
	CAPTURE,	//per captured territory
	DUMP,		//per ping pong ball in the center
	EXPLORE,	//per explored territory
	MINE		//per ping pong ball mined
}

unsigned char points[4];	//TODO make these the right values
points[CAPTURE] = 100;
points[DUMP]	= 60;
points[EXPLORE]	= 200;
points[MINE]	= 40
