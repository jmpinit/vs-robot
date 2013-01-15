//include in the umain loop

motor_set_vel(MOTOR_LEFT, -128);
motor_set_vel(MOTOR_RIGHT, 128);
pause(400);
motor_set_vel(MOTOR_LEFT, 128);
motor_set_vel(MOTOR_RIGHT, -128);
pause(400);

motor_brake(MOTOR_LEFT);
motor_brake(MOTOR_RIGHT);

while(true) { asm volatile ("NOP;"); }
