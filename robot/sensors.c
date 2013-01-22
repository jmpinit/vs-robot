#include <joyos.h>
#include "inc/util_math.h"
#include "inc/sensors.h"

static float gyro_transform = 0;

void gyro_zero(void) {
	gyro_transform = within(-180, gyro_get_degrees() - vps_get_degrees(), 180);
}

float gyro_absolute(void) {
	return within(-180, gyro_get_degrees() - gyro_transform, 180);
}

float vps_get_degrees(void) {
	float val = vps_heading;
	val /= 11.375;	//convert to degrees
	return val;
}

bool vps_is_shit(void) {
	return vps_x==0 && vps_y==0 && vps_heading==0;
}

float encoder_read_avg(void) {
	return (encoder_read(ENCODER_LEFT)+encoder_read(ENCODER_RIGHT))/2.0;
}

float vps_to_encoder(float dist) {
	return dist*TICKS_PER_VPS;
}
