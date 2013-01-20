//nav point teller
while(true) {
	while(!SWITCH_GO()) { NOTHING; }
	vps_update();
	bprintf("(%d, %d)\n", vps_x, vps_y);
}
