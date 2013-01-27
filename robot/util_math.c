#include <math.h>
#include "inc/util_math.h"

float within(float min, float val, float max) {
	while(val<min) val += max-min;
	while(val>max) val -= max-min;
	return val;
}

float bound(float min, float val, float max) {
	if(val<min) return min;
	if(val>max) return max;
	return val;
}

float distance(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x2-x1, 2)+pow(y2-y1, 2));
}

float max(float a, float b) {
	if(a>b) return a;
	return b;
}

float angle_between(int x1, int y1, int x2, int y2) {
	return (atan2(y2-y1, x2-x1)/M_PI)*180;
}
