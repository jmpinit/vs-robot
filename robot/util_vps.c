#include <joyos.h>
#include "util_vps.h"

float vps_angle_current(void) {
	copy_objects();
	float val = game.coords[0].theta;
	val /= 5.68888888889;	//convert to degrees
	return val;
}

pt vps_pos_us(void) {
	copy_objects();
	pt loc = { game.coords[0].x, game.coords[0].y };
	return loc;
}

pt vps_pos_them(void) {
	copy_objects();
	pt loc = { game.coords[1].x, game.coords[1].y };
	return loc;
}
