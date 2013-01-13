#ifndef _PID_
#define _PID_

typedef struct {
	//tweaked for straight motion
	float epsilon;
	float dt;
	float Kp, Kd, Ki;

	float pre_error;
} pid_data;

float pid_calc(pid_data prefs, float current, float target);

#endif
