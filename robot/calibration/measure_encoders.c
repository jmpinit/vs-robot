//include in the umain loop

/*
   Move forward in a straight line for a while and then
   check the encoders against the distance traveled on the
   VPS. Gives ratio between encoder ticks and VPS grid.
*/

float desired = gyro_get_degrees();	//keep going where we were pointed

//pid settings for moving straight
pid_data pid_set = {
	0.1,
	0.1,
	3.0,
	0.1,
	0.05,
	0
};

while(encoder_read(ENCODER_CENTER)<4096) {
	float motor_bias = frob_get_range(0, 255);

	float heading = gyro_get_degrees();
	float output = pid_calc(pid_set, heading, desired);

	motor_set_vel(MOTOR_LEFT, within(-255, motor_bias + output, 255));
	motor_set_vel(MOTOR_RIGHT, within(-255, motor_bias - output, 255));
}
