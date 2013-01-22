while(true) {
	int pos = frob_read_range(0, 511);
	servo_set_pos(1, pos);
	printf("pos is %d\n", pos);
	pause(100);
}
