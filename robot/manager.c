#include <joyos.h>
#include "inc/manager.h"
#include "inc/util_math.h"

bool team;

pt map[] = {
	{ -1200, 10 },
	{ -590, -1048 },
	{ 620, -1048 },
	{ 1200, 10 },
	{ 620, 1048 },
	{ -590, 1048}
};

/* scoring information */
enum {
	CAPTURE,	//per captured territory
	DUMP,		//per ping pong ball in the center
	EXPLORE,	//per explored territory
	MINE		//per ping pong ball mined
}

unsigned char points[4];
points[CAPTURE] = 100;
points[DUMP]	= 40;
points[EXPLORE]	= 30;
points[MINE]	= 40;
