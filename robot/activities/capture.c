#include <joyos.h>
#include "../inc/activities/capture.h"

#define MAX_SPEED      150
#define TURN_ACCEL      20

int currentspeed = 20;

void capture(int territory){
    while (abs(currentspeed)<MAX_SPEED && vps_get_owner(!team){
        
        if(team==TEAM_RED)
            currentspeed+=TURN_ACCEL;
        else
            currentspeed -=TURN_ACCEL;
        
        wheel_capture(currentspeed);
        
        pause(100);
        
    }
        
        
}