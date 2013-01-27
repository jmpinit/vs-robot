#include <joyos.h>
#include "../inc/manager.h"
#include "../inc/sensors.h"
#include "../inc/activities/capture.h"

#define CAPTURE_CURRENT	10

void capture(unsigned char id) {
    /*while (abs(currentspeed)<MAX_SPEED && vps_get_owner(territory)!=team) {
        if(team==TEAM_RED)
            currentspeed+=TURN_ACCEL;
        else
            currentspeed -=TURN_ACCEL;
        wheel_capture(currentspeed);
        pause(100);
    }*/

	printf("= capturing %d =\n", id);

	go_territory(id, 96);

	printf("at waypoint! turning...\n");

	nav_turn_to(map[id].heading_capture);
	printf("back facing capture!\n");
	printf("approaching...\n");

	//reverse until we hit something
	nav_set_velocity(-96);
	float current = 0;
	float lastcurrent = 0;
	for(;;) {
		current = motor_get_current_avg();
		printf("current=%f\n");	

		if(current>CAPTURE_CURRENT && lastcurrent>CAPTURE_CURRENT) break;

		lastcurrent = current;
	} 
	nav_stop();
	nav_set_velocity(-(MIN_SPEED-4));		//press against capture gears

	//attempt to capture
	printf("capturing!\n");
	for(unsigned int i=0; i<255; i++) {
		motor_set_vel(MOTOR_CAPTURE, (team==TEAM_BLUE)? i: -i);
		pause(5);
	}

	printf("spinning down\n");
	for(int i=255; i>=0; i--) {
		motor_set_vel(MOTOR_CAPTURE, (team==TEAM_BLUE)? i: -i);
		pause(2);
	}

	printf("backing up\n");
	nav_straight_stop(30, 96);
}
