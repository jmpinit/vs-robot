nav_set_velocity(-64);

int base = bot.x;

while(!digital_read(CONTACT_LEFT)&&!digital_read(CONTACT_RIGHT)) {
	printf("%d %f\n", -(bot.x-base), sharp_get_avg(SHARP_BACK));
	yield();
}

nav_stop();

while(true) { NOTHING; }
