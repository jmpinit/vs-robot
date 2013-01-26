while(true) {
	motor_set_vel(MOTOR_CAPTURE, frob_read_range(0, 254));
	yield();
}
