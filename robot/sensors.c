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

unsigned char vps_get_owner(unsigned char id) {
	if(id<6) return game.territories[id].owner;
	return 0;
}

unsigned char vps_get_balls(unsigned char id) {
	if(id<6) return game.territories[id].remaining;
	return 0;
}

unsigned char vps_get_rate(unsigned char id) {
	if(id<6) return game.territories[id].rate_limit;
	return 0;
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

unsigned char vps_get_territory(void) {
	unsigned char id = 0;
	float angle = within(-180, (atan2(bot.x, bot.y)/M_PI)*180, 180);
	if(angle>150||angle<-150)
		id = 0;
	else if(angle>-150 && angle<-90)
		id = 1;
	else if(angle>-90 && angle<-30)
		id = 2;
	else if(angle>-30 && angle<30)
		id = 3;
	else if(angle>30 && angle<90)
		id = 4;
	else if(angle>90 && angle<150)
		id = 5;

	return id;
}
