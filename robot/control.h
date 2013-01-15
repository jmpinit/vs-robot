#ifndef _CONTROL_
#define _CONTROL_

#define MOTOR_LEFT	0
#define MOTOR_RIGHT	1

typedef struct {
	float epsilon;
	float dt;
	float Kp, Kd, Ki;

	float pre_error;
} pid_data;

void move_to(int x, int y);
float pid_calc(pid_data prefs, float current, float target);

#endif
