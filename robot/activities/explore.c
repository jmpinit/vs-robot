#include <joyos.h>
#include "../inc/activities/explore.h"
#include "../inc/manager.h"
#include "../inc/debug.h"

#define EXPLORE_SPEED	96

void explore(void) {
	PRINT("exploring\n");
	int start;
	if(team==TEAM_BLUE) start = 3;
	else				start = 0;

	for(unsigned char id=1; id<6; id++) {
		int target = (id+start)%6;
		go_territory(target, EXPLORE_SPEED);
	}
}
