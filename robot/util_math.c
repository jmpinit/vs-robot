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

int dist(int x1, int y1, int x2, int y2) {
	int relX = x2-x1;
	int relY = y2-y1;

	return sqrt(relX*relX+relY*relY);
}
