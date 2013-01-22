#ifndef _SENSORS_
#define _SENSORS_

#include "util_math.h"

#define TICKS_PER_VPS	.07		//conversion between encoders and VPS

#define ENCODER_LEFT	24
#define ENCODER_RIGHT	25
#define ENCODER_CENTER	27

#define vps_update()	copy_objects()
#define vps_x			game.coords[0].x
#define vps_y			game.coords[0].y
#define vps_heading		game.coords[0].theta
#define vps_enemy_x		game.coords[1].x
#define vps_enemy_y		game.coords[1].y

float gyro_absolute(void);
void gyro_zero(void);
float vps_get_degrees(void);
bool vps_is_shit(void);
float encoder_read_avg(void);		//read the average of the two encoders
float vps_to_encoder(float vps);	//calculate the distance in VPS coords from encoder ticks

#endif
