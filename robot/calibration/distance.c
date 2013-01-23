while(true) {
	printf("[%d, %d, %d]\n", analog_read(9), analog_read(11), analog_read(13));
	pause(100);
}
