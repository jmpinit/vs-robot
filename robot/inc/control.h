#ifndef _CONTROL_
#define _CONTROL_

#include "util_math.h"

#define MOTOR_LEFT		0
#define MOTOR_RIGHT		1

#define ENCODER_LEFT	24
#define ENCODER_RIGHT	25
#define ENCODER_CENTER	27

/* nav controller data */
typedef struct {
	float velocity;			//current velocity
	float heading;			//current absolute heading

	int target_velocity;	//velocity to approach
	float target_heading;	//heading to approach

	float a;				//linear acceleration
	float w;				//radial acceleration
} nav_data;

static nav_data nav_settings;

/* PID controller data */
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
	bool obstructed;	//whether the robot is having difficulty moving
} robot;

extern robot bot; 

/*
   NAVIGATION SYSTEM
   -point a to point b
   -sensor info combinator
*/

int navigator(void);	//main thread
void tick_motion(void);	//smooth control of motors
void tick_state(void);	//combines info to guess where robot is

void nav_set_velocity(int v);
void nav_set_heading(float heading);
void nav_init(void);

float convert_encoder(int ticks);

void move_to(int x, int y);
float pid_calc(pid_data* prefs, float current, float target);

#endif
