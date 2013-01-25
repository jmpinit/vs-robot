while(true) {
	for(int i=1; i<255; i++) {
		motor_set_vel(MOTOR_LEFT, i);
		float start = get_time_us();
		encoder_reset(ENCODER_LEFT);
		while(get_time_us()-start<1000000) NOTHING;
		printf("%d %d\n", i, encoder_read(ENCODER_LEFT));
	}
}
