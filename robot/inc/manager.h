#ifndef _MANAGER_
#define _MANAGER_

typedef enum activity {
	CAPTURE,
	DUMP,
	EXPLORE,
	MINE
}

//pick the next 
activity act_pick(void);

#endif
