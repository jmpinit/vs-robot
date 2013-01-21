#ifndef _CONTROL_
#define _CONTROL_

#include "util_math.h"

#define MOTOR_LEFT		0
#define MOTOR_RIGHT		1

#define SERVO_GATE		0
#define SERVO_LEVER		1

#define ENCODER_LEFT	24
#define ENCODER_RIGHT	25
#define ENCODER_CENTER	27

/* PID controller data */
typedef struct {
	float epsilon;
	float dt;
	float Kp, Kd, Ki;

	float integral;
	float pre_error;
} pid_data;

/* robot state */
typedef struct {
	int x;
	int y;
	bool obstructed;		//whether the robot is having difficulty moving
	
	float velocity;			//current velocity
	float heading;			//current absolute heading

	int target_velocity;	//velocity to approach
	float target_heading;	//heading to approach

	float a;				//linear acceleration
	float w;				//radial acceleration
} robot;

extern robot bot; 

/*
   MISC ACTUATION
*/

void gate_open(void);
void gate_close(void);
void lever_up(void);
void lever_middle(void);
void lever_down(void);

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

void nav_straight(int distance, int v);
void nav_turn_to(float heading);

float encoder_read_avg(void);		//read the average of the two encoders
float vps_to_encoder(float vps);	//calculate the distance in VPS coords from encoder ticks

void move_to_ptp(int x, int y);
void move_to(int x, int y);
float pid_calc(pid_data* prefs, float current, float target);

#endif
