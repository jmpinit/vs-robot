#include <math.h>
#include "inc/util_math.h"

int within(int min, int val, int max) {
	if(val<min) return min;
	if(val>max) return max;
	return val;
}

float bound(float min, float val, float max) {
	while(val<min) val += (max-min);
	while(val>max) val -= (max-min);
	return val;
}

float distance(int x1, int y1, int x2, int y2) {
	return sqrt(pow(x2-x1, 2)+pow(y2-y1, 2));
}
