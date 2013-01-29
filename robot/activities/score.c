#include <joyos.h>
#include "../inc/activities/score.h"
#include "../inc/manager.h"
#include "../inc/sensors.h"
#include "../inc/control.h"

#define SCORE_CURRENT   10
#define FUDGE           10

void score(unsigned char id) {
    printf("= scoring %d =\n", id);
    
    go_territory(id, 96);
    
    printf("at waypoint! turning...\n");
    
	//TODO turn towards target
    
    //go until we hit the inside wall
    nav_set_velocity(-96);
    float current = 0;
    float lastcurrent = 0;
    for(;;) {
        current = motor_get_current_avg();
        printf("current = %f\n");
        
        if(current>SCORE_CURRENT && lastcurrent>SCORE_CURRENT) break;
        
        lastcurrent = current;
    }
    nav_stop();
    
    //Dump!
    printf("dumping!\n");
    gate_open();
    pause(1500);
    gate_close();
    
    
    printf("backing up\n");
    //not sure the notation here?
    go_territory(id, 96);
}
