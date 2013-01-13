#ifndef _UTIL_VPS_
#define _UTIL_VPS_

typedef struct {
	float x;
	float y;
} pt;

float vps_angle_current(void);
pt vps_pos_us(void);
pt vps_pos_them(void);

#endif
