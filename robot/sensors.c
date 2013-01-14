#include <joyos.h>
#include "sensors.h"

void gyro_zero(void) {
	gyro_transform = gyro_get_degrees() - vps_get_degrees();	
}

float gyro_absolute(void) {
	return gyro_get_degrees() - gyro_transform;
}

float vps_get_degrees(void) {
	float val = vps_heading;
	val /= 11.375;	//convert to degrees
	return val;
}
