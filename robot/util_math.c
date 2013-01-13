#include "util_math.h"

int within(int min, int val, int max) {
	if(val<min) return min;
	if(val>max) return max;
	return val;
}

float bound(float val, float max) {
	if(val<max) return val;
	
	while(val>max)
		val -= max;

	return val;
}

