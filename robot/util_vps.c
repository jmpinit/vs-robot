#include <joyos.h>
#include "util_vps.h"

float vps_get_degrees(void) {
	float val = vps_heading;
	val /= 11.375;	//convert to degrees
	return val;
}
