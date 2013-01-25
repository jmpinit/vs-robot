#include <joyos.h>
#include <math.h>
#include "inc/manager.h"
#include "inc/sensors.h"
#include "inc/control.h"
#include "inc/util_math.h"
#include "inc/debug.h"

#define BALLS_MAX 15

int act;
int territory_pts;
int best_territory;
int best_territory_pts;

void play(void){ //ideally can we just loop this the whole game? put it in umain or something
    switch act {
    case 1:
        //circle(radius, 255 ideally)
        break;
    case 2:
        //circle finished to 20s left, dump balls if too many.
        if (vps_get_owner(get_territory())!=team){
            capture_territory(get_territory);
        }
        
        else if (vps_get_balls(get_territory))>0){
            mine_balls(get_territory);
        }
        
        else {
            //move to different territory NOT containg opponent
            move_to(rank_territories);
        }
        break;
    case 3:
        //emergency ball dump before end
        //dump balls anyway even if we think we don't have any?
        break
    }
}

int rank_territories(void){
    for (int i=0; i<6; i++){
        
        territory_pts = 0; // reset territory score
        
        //find territories you/enemy aren't in, penalize them below anything else
        if(i==get_territory() || i==enemy.territory())
            territory_pts = -200;
        else {
            if (vps_get_owner(i)==!team)
                territory_pts+=100; //points if we cap territory
            territory_pts+=40*vps_get_balls; //change to 80 to include dumping?
            territory_pts-=30*(i % 6); //penalizing farther away
            
        }
        if(territory_pts>best_territory_pts){
            best_territory_pts = territory_pts; //pick best
            best_territory = i;
        }
            
    }
    return best_territory;
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
    act = 1;
    territory_pts = 0;
    best_territory = 0;
    best_territory_pts = 0;
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

void visit_one(void) {
	unsigned char id = get_territory();
	territory this = map[id];
	move_to_ptp(this.center.x, this.center.y, 96);
	while(vps_is_shit()) { vps_update(); yield(); }
	nav_turn_to(this.heading_mine);

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

	bprintf("backing up\n");
	nav_straight_stop(30, -96);
}

void manager_explore(int vel) {
	bprintf("exploring\n");
	for(unsigned char id=0; id<6; id++) {
		move_to_ptp(map[(id+2)%6].center.x, map[(id+2)%6].center.y, vel);
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
