#include <joyos.h>
#include <math.h>
#include "inc/manager.h"
#include "inc/sensors.h"
#include "inc/control.h"
#include "inc/util_math.h"
#include "inc/debug.h"

territory map[6] = {
	//center		mine			capture			mine angle	capture angle
	{{-1200, 10},	{-1688, 341},	{-1773, -355},	149.275,	27.25},
	{{-590, -1048},	{-1165, -1322},	{-593, -1733},	-151.6,		83},
	{{620, -1048},	{575, -1670}, 	{1180, -1360},	-93,		147.7},
	{{1200, 10},	{1664, -343},	{1711, 388},	-32.8,		-151.9},
	{{620, 1048},	{1110, 1300},	{555, 1697},	25.80,		-93.27},
	{{-590, 1048},	{-555, 1634},	{-1200, 1370},	83.956,		-33.85}
};

/* scoring information */
enum {
	CAPTURE,	//per captured territory
	DUMP,		//per ping pong ball in the center
	EXPLORE,	//per explored territory
	MINE		//per ping pong ball mined
};

unsigned char points[4];

void manager_init(void) {
	points[CAPTURE] = 100;
	points[DUMP]	= 40;
	points[EXPLORE]	= 30;
	points[MINE]	= 40;
}

void visit_one(void) {
	unsigned char id = vps_get_territory();
	territory this = map[id];
	move_to_ptp(this.center.x, this.center.y, 96);
	while(vps_is_shit()) { vps_update(); yield(); }
	nav_turn_to(this.heading_capture);
}

void manager_explore(void) {
	bprintf("exploring\n");
	for(unsigned char id=0; id<6; id++) {
		move_to_ptp(map[(id+2)%6].center.x, map[(id+2)%6].center.y, 245);
	}
}

void manager_visit(void) {
	for(unsigned char id=0; id<6; id++) {
		territory this = map[(id+2)%6];
		bprintf("visiting %d\n", (id+2)%6);
		move_to_ptp(this.center.x, this.center.y, 96);
		bprintf("at waypoint\n");

		while(vps_is_shit()) { vps_update(); yield(); }
		nav_turn_to(this.heading_mine);
		bprintf("approaching gears\n");
		nav_straight_stop(10, 96);

		pause(1000);

		bprintf("backing up\n");
		nav_straight_stop(10, -96);
	}	
}
