#ifndef _CONTROL_
#define _CONTROL_

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

int motor_controller(void);
void nav_set_speed(int speed);
void nav_set_heading(float heading);
void nav_init(void);

void move_to(int x, int y);
float pid_calc(pid_data prefs, float current, float target);

#endif
