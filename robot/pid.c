#include <joyos.h>
#include "pid.h"

float pid_calc(pid_data prefs, float current, float target) {
	float error = (target - current);

	float integral = 0;
	if(abs(error) > prefs.epsilon)
		integral = integral + error*prefs.dt;
	float derivative = (error - prefs.pre_error)/prefs.dt;
	float output = prefs.Kp*error + prefs.Ki*integral + prefs.Kd*derivative;

	//Update error
	prefs.pre_error = error;

	return output;
}
