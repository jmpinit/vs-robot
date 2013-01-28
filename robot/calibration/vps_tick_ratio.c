int start = encoder_read_avg();

vps_update();
int startx = vps_x;
int starty = vps_y;

nav_set_velocity(96);
while(encoder_read_avg()-start<128) {
	NOTHING;
	yield();
}

nav_stop();

vps_update();
int endx = vps_x;
int endy = vps_y;

int end = encoder_read_avg();
int ticks = end-start;

float dist = distance(startx, starty, endx, endy);

float ratio = ticks/dist;

printf("ticks=%d dist=%f ratio=%f", ticks, dist, ratio);

halt();
