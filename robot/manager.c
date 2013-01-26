#include <joyos.h>
#include <math.h>

#include "inc/manager.h"

#include "inc/sensors.h"
#include "inc/control.h"
#include "inc/util_math.h"
#include "inc/debug.h"

#include "activities/capture.c"
#include "activities/dump.c"
#include "activities/explore.c"
#include "activities/mine.c"

#define MOVE_SPEED	96
#define BALLS_MAX	15

int act;

int get_best(void);

void play(void) {
	switch(act) {
		case 0:
			//exploration
			circle(1024, 255);
			break;
		case 1:
			//circle finished to 20s left, dump balls if too many.
			vps_update();
			if(vps_get_owner(bot.territory)!=team) {		//if not ours yet
				capture(bot.territory);			//capture it
			} else if(vps_get_balls(bot.territory)>0) {	//if it is ours and there are balls
				mine(bot.territory);						//get the balls
			} else {
				//move to different territory NOT containg opponent
				go_territory(get_best(), MOVE_SPEED);
			}
			break;
		case 2:
			//TODO emergency ball dump before end
			//dump balls anyway even if we think we don't have any?
			break;
	}
}

int get_best(void){
	unsigned char best = 0;
	unsigned char best_pts = 0;

    for(int id=0; id<6; id++) {
        int pts = 0;
        
        //find territories you/enemy aren't in, penalize them below anything else
        if(id==bot.territory || id==other_bot.territory) {
            pts = -200;
		} else {
            if(vps_get_owner(id)==!team)	//is it ours?
                pts += 100;					//points if we cap territory
            pts += 40*vps_get_balls(id);	//change to 80 to include dumping?
            pts -= 30*(id % 6);				//penalize for being far away
            
        }

        if(pts>best_pts){
            best_pts = pts; //pick best
            best = id;
        }
            
    }
    return best;
}

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

    act = 0;
}

void circle(unsigned int r, int vel) {
	pid_data pid_circle;
	pid_circle.epsilon	= 0.01;
	pid_circle.dt		= 0.01;
	pid_circle.Kp		= 0.01;
	pid_circle.Kd		= 0.002;
	pid_circle.Ki		= 0.0001;

	float angle = 360.0*atan2(bot.y, bot.x)/(2.0*M_PI);
	float start = angle-16;
	nav_turn_to(angle+90);
	nav_set_velocity(vel);

	while(within(0, angle-start, 360)>10) {
		angle = 360.0*atan2(bot.y, bot.x)/(2.0*M_PI);
		nav_set_heading(angle+90);
		yield();
	}
}

void visit_one(unsigned char id) {
	printf("visiting %d\n", id);

	go_territory(id, 96);

	printf("at waypoint! turning...\n");

	nav_turn_to(map[id].heading_mine);
	printf("facing mine!\n");
	printf("approaching...\n");

	//forward until we hit something
	nav_set_velocity(96);
	while(!digital_read(CONTACT_LEFT) && !digital_read(CONTACT_RIGHT));

	//brake TODO make nav function
	nav_stop();
	//printf("at mine!\n");

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
	nav_straight_stop(30, -96);
}

void manager_explore(int vel) {
	bprintf("exploring\n");
	for(unsigned char id=0; id<6; id++) {
		go_to(map[(id+2)%6].center.x, map[(id+2)%6].center.y, vel);
	}
}

void manager_visit(void) {
	for(unsigned char id=0; id<6; id++) {
		territory this = map[(id+2)%6];
		bprintf("visiting %d\n", (id+2)%6);
		go_to(this.center.x, this.center.y, 96);
		bprintf("at waypoint\n");

		while(vps_is_shit()) { vps_update(); yield(); }
		nav_turn_to(this.heading_mine);
		bprintf("approaching mine\n");

		//forward until we hit something
		nav_set_velocity(96);
		while(!digital_read(CONTACT_LEFT) && !digital_read(CONTACT_RIGHT));

		//make sure that we are square
		if(!digital_read(CONTACT_LEFT) && digital_read(CONTACT_RIGHT)) {
			while(!digital_read(CONTACT_LEFT)) {
				nav_set_heading(bot.target_heading-1);
				pause(10);
			}
		}

		if(digital_read(CONTACT_LEFT) && !digital_read(CONTACT_RIGHT)) {
			while(!digital_read(CONTACT_RIGHT)) {
				nav_set_heading(bot.target_heading+1);
				pause(10);
			}
		}

		//attempt to mine
		lever_middle();
		for(int i=0; i<5; i++) {
			lever_down();
			pause(300);
			lever_middle();
			pause(300);
		}
		pause(500);
			
		pause(1000);

		bprintf("backing up\n");
		nav_straight_stop(30, -96);
	}
}
