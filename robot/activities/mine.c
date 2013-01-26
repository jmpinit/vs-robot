#include <joyos.h>
#include "../inc/activities/mine.h"

#define MINE_SPEED 300 //in milliseconds
#define TIMEOUT_TRIES 3

void mine(unsigned char territory){
    int timestried = 0;
    int ballsremaining = 0;
    
    
    while(vps_get_balls(territory)>0 && timestried<TIMEOUT_TRIES){
        
        ballsremaining = vps_get_balls(territory);
        lever_down();
        pause(MINE_SPEED);
        lever_middle();
        pause(MINE_SPEED);
        if (ballsremaining==vps_get_balls(territory))timestried++;
        
    }
    lever_up();
}
