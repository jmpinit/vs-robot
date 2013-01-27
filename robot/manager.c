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
unsigned char points[4];

int get_best(void);

void play(void) {
	/*points[CAPTURE] = 100;
	points[DUMP]	= 40;
	points[EXPLORE]	= 30;
	points[MINE]	= 40;*/

    act = 0;

	for(unsigned char i=0; i<6; i++) capture(i);
	halt();

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
	capture(id);
	mine(id);
}

void manager_explore(int vel) {
	bprintf("exploring\n");
	for(unsigned char id=0; id<6; id++) {
		go_to(arena[(id+2)%6].center.x, arena[(id+2)%6].center.y, vel);
	}
}
