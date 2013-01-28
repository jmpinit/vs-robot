#include <joyos.h>
#include "../inc/activities/mine.h"
#include "../inc/control.h"

#define TRIES			3
#define MINE_TIMEOUT	3	//time until failure (sec)
#define MINE_SPEED		96

void wall_square(void) {
	//square ourselves
	nav_off();
	long start = get_time_us();
	while(!(digital_read(CONTACT_LEFT) && digital_read(CONTACT_RIGHT)) && (get_time_us()-start<MINE_TIMEOUT*1000000)) {
		if(digital_read(CONTACT_LEFT)) {
			motor_set_vel(MOTOR_LEFT, -(MIN_SPEED+10));
			motor_set_vel(MOTOR_RIGHT, 64);
		}
		if(digital_read(CONTACT_RIGHT)) {
			motor_set_vel(MOTOR_RIGHT, -(MIN_SPEED+10));
			motor_set_vel(MOTOR_LEFT, 64);
		}
	}
	nav_on();
}

void mine(unsigned char id) {
	PRINT("= mining %d =\n", id);

	go_territory(id, MINE_SPEED);

	PRINT("at waypoint! turning...\n");

	nav_turn_to(within(-180, angle_between(bot.x, bot.y, arena[id].mine.x, arena[id].mine.y)+arena[id].heading_mine, 180));
	PRINT("facing mine!\n");
	PRINT("approaching...\n");

	//forward until we hit something
	nav_set_velocity(MINE_SPEED);
	float start = get_time_us();
	while(!digital_read(CONTACT_LEFT) && !digital_read(CONTACT_RIGHT) && (get_time_us()-start<MINE_TIMEOUT*1000000)) { NOTHING; yield(); }
	nav_stop();

	wall_square();

	//attempt to mine
	PRINT("mining!\n");
	lever_middle();
	for(int i=0; i<5; i++) {
		lever_down();
		pause(300);
		lever_middle();
		pause(300);
	}
	pause(500);

	PRINT("backing up\n");
	nav_straight(30, -MINE_SPEED);

	lever_up();
}
