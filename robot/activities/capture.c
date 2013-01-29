#include <joyos.h>
#include "../inc/activities/capture.h"
#include "../inc/manager.h"
#include "../inc/debug.h"
#include "../inc/control.h"
#include "../inc/sensors.h"

#define CAP_SPEED		128
#define CAP_TIME		3000		//how long to spin the gears in millis
#define PUSH			MIN_SPEED-12	//vel for pushing against gears
#define CAP_TIMEOUT		10			//seconds until failure
#define RETRIES			1
#define CAPTURE_CURRENT	12

bool drive_till_overcurrent(void) {
	long start = get_time_us();

	float current = 0;
	float lastcurrent = 0;
	for(;;) {
		current = motor_get_current_avg();
		PRINT("current=%f\n", current);

		if(current>CAPTURE_CURRENT && lastcurrent>CAPTURE_CURRENT) break;
		if(get_time_us()-start>CAP_TIMEOUT*1000000) return false;

		lastcurrent = current;
	} 
	nav_stop();

	return true;
}

void capture(unsigned char id) {
	PRINT("= capturing %d =\n", id);

	go_territory(id, CAP_SPEED);

	PRINT("at waypoint! turning...\n");

	nav_turn_to(within(-180, angle_between(bot.x, bot.y, arena[id].capture.x, arena[id].capture.y)+180+arena[id].heading_capture, 180));
	PRINT("back facing capture!\n");
	PRINT("approaching...\n");

	//reverse until we hit something
	nav_set_velocity(-CAP_SPEED);
	drive_till_overcurrent();
	nav_set_velocity(-PUSH);		//press against capture gears

	//attempt to capture
	PRINT("capturing!\n");
	for(unsigned int i=0; i<255; i++) {
		motor_set_vel(MOTOR_CAPTURE, (team==TEAM_BLUE)? i: -i);
		pause(5);
	}

	motor_set_vel(MOTOR_CAPTURE, (team==TEAM_BLUE)? 255: -255);

	pause(CAP_TIME);

	//retry if necessary
	int try = 0;
	while(arena[bot.territory].owner!=robot_id && try<RETRIES) {
		nav_straight(20, CAP_SPEED);
		nav_turn_to(within(-180, angle_between(bot.x, bot.y, arena[id].capture.x, arena[id].capture.y)+180, 180));
		nav_set_velocity(-CAP_SPEED);
		drive_till_overcurrent();
		nav_set_velocity(-PUSH);		//press against capture gears
		pause(CAP_TIME);
		try++;	
	}

	PRINT("spinning down\n");
	for(int i=255; i>=0; i--) {
		motor_set_vel(MOTOR_CAPTURE, (team==TEAM_BLUE)? i: -i);
		pause(2);
	}

	PRINT("backing up\n");
	nav_straight(30, CAP_SPEED);
}
