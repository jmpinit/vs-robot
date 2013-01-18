#ifndef _CONTROL_
#define _CONTROL_

#include "util_math.h"

#define MOTOR_LEFT		0
#define MOTOR_RIGHT		1

#define ENCODER_LEFT	0
#define ENCODER_RIGHT	1
#define ENCODER_CENTER	3

//NAV CONTROLLER DATA
typedef struct {
	float speed;
	int target;
	float heading;

	float slope;
} nav_data;

//PID CONTROLLER DATA
typedef struct {
	float epsilon;
	float dt;
	float Kp, Kd, Ki;

	float pre_error;
} pid_data;

/* robot state */
typedef struct {
	int x;
	int y;
	float heading;
} robot;

extern robot bot; 

/*
   NAVIGATION SYSTEM
   -point a to point b
   -sensor info combinator
*/

int navigator(void);	//main thread
void tick_speed(void);	//smooth control of motors
void tick_state(void);	//combines info to guess where robot is

void nav_set_speed(int speed);
void nav_set_heading(float heading);
void nav_init(void);

float convert_encoder(int ticks);

void move_to(int x, int y);
float pid_calc(pid_data prefs, float current, float target);

#endif
