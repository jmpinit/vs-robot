#include <joyos.h>
#include <math.h>

#include "util_math.h"
#include "pid.h"
#include "util_vps.h"

#define TRUE	1
#define FALSE	0

#define GYRO_PORT		8
#define LSB_US_PER_DEG	1400000

#define MOTOR_LEFT	0
#define MOTOR_RIGHT	1

#define MAX_SPEED	80

float gyro_transform;

int vps_vals[3];
float vps_avg;

pid_data pid_linear_settings = {
	0.1,
	0.1,
	3.0,
	0.1,
	0.05,
	0
};

void led_set(unsigned char led, unsigned char state) {
	if(led<3) {
		if(state)
			PORTD |= 1<<(led+2);
		else
			PORTD &= ~(1<<(led+2));
	}
}

void led_init(void) {
	//create outputs for LEDs
	DDRD = (1<<PD2)|(1<<PD3)|(1<<PD4);
	//turn off the LEDS
	for(unsigned char i=0; i<3; i++) led_set(i, FALSE);
}

void vps_track(void) {
	float total = 0;
	for(int i=0; i<3; i++)
		total += vps_vals[i];

	vps_avg = total/3.0;
}

int usetup(void) {
	led_init();
	led_set(0, 1);
	led_set(1, 1);
	led_set(2, 1);

	//set our team # for the VPS
	extern volatile uint8_t robot_id;
	robot_id = 12;

	gyro_init(GYRO_PORT, LSB_US_PER_DEG, 500L);
	gyro_transform = gyro_get_degrees()-vps_angle_current();

	led_init();

	return 0;
}

//attempts to take robot to VPS coordinate
void move_to(int x, int y) {
	copy_objects();
	float desired = (atan2(y-game.coords[0].y, x-game.coords[0].x)/M_PI)*180;

	printf("desired: %f", desired);

	float current_dist;
	int old_x = game.coords[0].x;
	int old_y = game.coords[0].y;

    do {
		current_dist = distance(game.coords[0].x, game.coords[0].y, x, y);

		int motor_bias;
		if(current_dist>512) 
			motor_bias = frob_read_range(0, 245);
		else
			motor_bias = current_dist/(512.0/frob_read_range(0, 245))+32;

		float heading = gyro_get_degrees()-gyro_transform; //TODO fuse VPS and gyro data
		float output = pid_calc(pid_linear_settings, heading, desired);

		if(output>32)
			output = 32;
		if(output<-32)
			output = -32;

		motor_set_vel(MOTOR_LEFT, within(0, motor_bias + output, 255));
		motor_set_vel(MOTOR_RIGHT, within(0, motor_bias - output, 255));
		
		//update where we are	
		copy_objects();
		if(abs(output)<5.0) { //TODO check if VPS is lying
			printf("updated gyro");
			gyro_transform = gyro_get_degrees()-vps_angle_current(); 
		}
		
		int dx = game.coords[0].x-old_x;
		int dy = game.coords[0].y-old_y;

		if(abs(output)<5.0) {
			printf("updated pos");
			desired = (atan2(y-(game.coords[0].y+dy), x-(game.coords[0].x+dx))/M_PI)*180;
		}

		printf("%f\n", output);

		old_x = game.coords[0].x;
		old_y = game.coords[0].y;

		//get our new location
		copy_objects();
		//printf("dist=%f	", distance(game.coords[0].x, game.coords[0].y, x, y));
		//printf("(%d, %d) @ %f\n", game.coords[0].x, game.coords[0].y, heading);
	} while(current_dist>128);
}

int umain (void) {
	led_set(0, TRUE);
	pause(100);
	led_set(1, TRUE);	//about to start!
	pause(100);
	led_set(2, TRUE);	//we're on our way
	pause(100);

    while(1) {
		//movement test
		while(1) {
			printf("moving to first pt.");
			copy_objects();
			move_to(game.coords[1].x, game.coords[1].y);
			printf("i think i am there.");
			motor_set_vel(0, 0);
			motor_set_vel(1, 0);
			pause(1000);
		}
	}

    return 0;
}
