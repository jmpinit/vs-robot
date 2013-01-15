//include in the umain loop

/*
   Move forward in a straight line for a while and then
   check the encoders against the distance traveled on the
   VPS. Gives ratio between encoder ticks and VPS grid.
*/

float desired = gyro_get_degrees();	//keep going where we were pointed

ctrl_set_heading(desired);
ctrl_set_speed(128);

while(encoder_read(ENCODER_LEFT)<64) {
	asm volatile ("NOP");
}
