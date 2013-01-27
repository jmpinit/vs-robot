#ifndef _UTIL_MATH_
#define _UTIL_MATH_

typedef struct {
	float x;
	float y;
} pt;

float distance(int x1, int y1, int x2, int y2);
float within(float min, float val, float max);
float bound(float min, float val, float max);
float max(float a, float b);
float angle_between(int x1, int y1, int x2, int y2);

#endif
