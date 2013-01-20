while(true) {
	int pos = frob_read_range(0, 255);
	servo_set_pos(0, pos);
	if(!SWITCH_GO()) {
		printf("pos is %d", pos);
		pause(300);
	}
}
