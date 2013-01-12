struct pid_data {
	//tweaked for straight motion
	float epsilon;
	float dt;
	float Kp, Kd, Ki;

	float pre_error;
};

pid_data pid_linear_data = {
	0.1,
	0.1,
	3.0,
	0.5,
	0.1,
	0
};

float pid_linear_calc(pid_data* prefs, float current, float target) {
	pid_data* p = &pid_linear_data;

	float error = (target - current);

	float integral = 0;
	if(abs(error) > p.epsilon)
		integral = integral + error*p.dt;
	derivative = (error - p.pre_error)/p.dt;
	output = p.Kp*error + p.Ki*integral + p.Kd*derivative;

	//Update error
	p.pre_error = error;

	return output;
}
