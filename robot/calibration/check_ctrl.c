//include in the umain loop

while(true) {
	ctrl_set_speed(255);
	printf("set speed");
}
//pause(2000);
//ctrl_set_speed(0);

while(true) { led_clear(); asm volatile ("NOP"); }
