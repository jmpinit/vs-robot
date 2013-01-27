#include <joyos.h>
#include "../inc/activities/mine.h"

#define TRIES		3
#define SPEED		96

void mine(unsigned char id) {
	printf("= mining %d =\n", id);

	go_territory(id, SPEED);

	printf("at waypoint! turning...\n");

	nav_turn_to(map[id].heading_mine);
	printf("facing mine!\n");
	printf("approaching...\n");

	//forward until we hit something
	nav_set_velocity(96);
	while(!digital_read(CONTACT_LEFT) && !digital_read(CONTACT_RIGHT)) { NOTHING; yield(); }
	nav_stop();

	//attempt to mine
	printf("mining!\n");
	lever_middle();
	for(int i=0; i<5; i++) {
		lever_down();
		pause(300);
		lever_middle();
		pause(300);
	}
	pause(500);

	printf("backing up\n");
	nav_straight_stop(30, SPEED);
}
