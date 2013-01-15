//include in the umain loop

*target_speed = 255;
pause(2000);
*target_speed = 0;

while(true) { led_clear(); asm volatile ("NOP"); }
