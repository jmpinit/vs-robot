#include "stdio.h"
#include "time.h"

/*
   A test of various struct manipulations
*/

typedef struct {
	float x;
	float y;
} pt;

pt random_pt(void) {
	float x = (float)(rand()%100);
	float y = (float)(rand()%100);
	printf("<random_pt()>: (%f, %f)\n", x, y);

	pt loc = { x, y };
	return loc;
}

void main() {
	srand(time(NULL));

	pt the_pt = random_pt();
	printf("<main()>: point is (%f, %f)\n", the_pt.x, the_pt.y);
}

