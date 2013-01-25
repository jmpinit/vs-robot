#ifndef _SENSORS_
#define _SENSORS_

#include "util_math.h"

#define TICKS_PER_VPS	0.07		//conversion between encoders and VPS
#define VEL_SLOPE		0.279972	// ticks/sec = vel_slope * motor_vel

#define ENCODER_LEFT	24
#define ENCODER_RIGHT	25
#define ENCODER_CENTER	27

#define SHARP_LEFT		10
#define SHARP_RIGHT		12
#define SHARP_BACK		14

#define CONTACT_LEFT	1
#define CONTACT_RIGHT	3

/* VPS INFO */
#define vps_update()	copy_objects()

#define vps_x			game.coords[0].x
#define vps_y			game.coords[0].y
#define vps_heading		game.coords[0].theta

#define vps_score		game.coords[0].score

#define vps_enemy_x		game.coords[1].x
#define vps_enemy_y		game.coords[1].y

int sensor(void);
void sense_init(void);

float sharp_get_avg(unsigned char id);

float gyro_absolute(void);
void gyro_zero(void);

float vps_get_degrees(void);					//get the heading in degrees
unsigned char vps_get_owner(unsigned char id);	//get the owner of a specified territory
unsigned char vps_get_balls(unsigned char id);	//get ow many balls are remaining in a territory
unsigned char vps_get_rate(unsigned char id);	//get rate info for specified territory
unsigned char get_territory(void);				//what territory are we in?
bool vps_is_shit(void);							//whether the latest VPS values are valid

float encoder_read_avg(void);		//read the average of the two encoders
float vps_to_encoder(float vps);	//calculate the distance in encoder ticks from VPS coords
float encoder_to_vps(int ticks);	//calculate the distance in VPS coords from encoder ticks

#endif
