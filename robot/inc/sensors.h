#ifndef _SENSORS_
#define _SENSORS_

#include "util_math.h"

#define vps_update()	copy_objects()
#define vps_x			game.coords[0].x
#define vps_y			game.coords[0].y
#define vps_heading		game.coords[0].theta
#define vps_target_x	game.coords[1].x
#define vps_target_y	game.coords[1].y

float gyro_absolute(void);
void gyro_zero(void);
float vps_get_degrees(void);
bool vps_is_shit(void);

#endif
