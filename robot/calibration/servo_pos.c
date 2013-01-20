while(true) {
	int pos = frob_read_range(0, 511);
	servo_set_pos(0, pos);
	if(!SWITCH_GO()) {
		printf("pos is %d\n", pos);
		pause(300);
	}
}
