#ifndef _CONTROL_
#define _CONTROL_

#include "util_math.h"

#define MIN_SPEED		35
#define CLOSE_ENOUGH	512

#define MOTOR_LEFT		0
#define MOTOR_RIGHT		1
#define MOTOR_CAPTURE	2

#define SERVO_GATE		1
#define SERVO_LEVER		0

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
	unsigned char territory;

	bool obstructed;		//whether the robot is having difficulty moving
	
	float velocity;			//current velocity
	float real_velocity;	//current sensed velocity (via vps, encoders, etc.)
	float heading;			//current absolute heading

	int target_velocity;	//velocity to approach
	float target_heading;	//heading to approach

	float accel;			//linear acceleration
	float deccel;			//linear decceleration
	float w;				//radial acceleration

	float gyro_transform;	//constant offset of gyro from VPS heading
} robot;

typedef struct {
	int x;
	int y;
	unsigned char territory;
} enemy;

extern robot bot; 
extern enemy other_bot;

/*
   MISC ACTUATION
*/

void gate_open(void);
void gate_close(void);
void lever_up(void);
void lever_middle(void);
void lever_down(void);
void capture_red(void);
void capture_blue(void);
void wheel_capture(int turnspeed);

/*
   NAVIGATION SYSTEM
   -point a to point b
   -sensor info combinator
*/

int navigator(void);	//main thread
void tick_motion(void);	//smooth control of motors

void nav_set_velocity(int v);
void nav_set_heading(float heading);
void nav_stop(void);
void nav_init(void);

void nav_straight(int distance, int v);
void nav_straight_stop(int distance, int v);
void nav_turn_to(float heading);

void go_territory(unsigned char target, int vel);
void go_to(int x, int y, int vel);
float pid_calc(pid_data* prefs, float current, float target);

#endif
