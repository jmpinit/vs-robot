#ifndef _CONTROL_
#define _CONTROL_

#define MOTOR_LEFT		0
#define MOTOR_RIGHT		1

#define ENCODER_LEFT	0
#define ENCODER_RIGHT	1
#define ENCODER_CENTER	3

typedef struct {
	float current;
	int target;
	float slope;
} ramper_settings;

typedef struct {
	float epsilon;
	float dt;
	float Kp, Kd, Ki;

	float pre_error;
} pid_data;

int ramper_thread(void);
void move_to(int x, int y);
float pid_calc(pid_data prefs, float current, float target);

#endif
