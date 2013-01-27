#include <joyos.h>
#include "../inc/activities/explore.h"

void explore(void) {
	bprintf("exploring\n");
	for(unsigned char id=0; id<6; id++) {
		go_to(arena[(id+2)%6].center.x, arena[(id+2)%6].center.y, vel);
	}
}
