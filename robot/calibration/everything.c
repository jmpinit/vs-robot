/* GYROSCOPE */
PRINT("rotate the robot.\n");
float start = gyro_get_degrees();
while(abs(gyro_get_degrees()-start)<45) NOTHING;

/* MOTORS */
PRINT("moving left motor.\n");
motor_set_vel(MOTOR_LEFT, 128);
pause(500);
motor_set_vel(MOTOR_LEFT, -128);
motor_brake(MOTOR_LEFT);
pause(500);

PRINT("moving right motor.\n");
motor_set_vel(MOTOR_RIGHT, 128);
pause(500);
motor_set_vel(MOTOR_RIGHT, -128);
motor_brake(MOTOR_RIGHT);
pause(500);

PRINT("moving capture motor.\n");
motor_set_vel(MOTOR_CAPTURE, 64);
pause(1000);
motor_set_vel(MOTOR_CAPTURE, -64);
pause(1000);
motor_set_vel(MOTOR_CAPTURE, 0);

/* SERVOS */
PRINT("moving lever.\n");
lever_up();
pause(500);
lever_down();
pause(1000);

PRINT("moving gate.\n");
gate_open();
pause(500);
gate_close();
pause(1000);

/* CONTACTS */
PRINT("press the left contact.\n");
while(!digital_read(CONTACT_LEFT)) NOTHING;
PRINT("press the right contact.\n");
while(!digital_read(CONTACT_RIGHT)) NOTHING;

halt();
