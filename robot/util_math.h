#ifndef _UTIL_MATH_
#define _UTIL_MATH_

typedef struct {
	float x;
	float y;
} pt;

int within(int min, int val, int max);
float bound(float val, float max);

#endif
