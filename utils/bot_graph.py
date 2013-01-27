import sys
import serial
import pygame

port = '/dev/rfcomm0'

if(len(sys.argv)==2):
	port = sys.argv[1]
else:
	print 'defaulting to '+port

screen = pygame.display.set_mode((1024, 600), pygame.FULLSCREEN)
ser = serial.Serial(port, 19200, timeout = 1)

points = list()

while 1:
	line = ser.readline().strip()
	parts = line.split(" ")
	if(len(parts)==2):
		try:
			x = (int(parts[0])+2048)/10.0
			y = (-int(parts[1])+2048)/10.0
			points.append((x, y));
		except:
			print "error: "+line
	
	for pt in points:
		pygame.draw.rect(screen, (255, 255, 255), (pt[0], pt[1], 8, 8))

	pygame.display.flip()
	surface.fill((0, 0, 0));

	for event in pygame.event.get():
		if(event.type==pygame.KEYDOWN):
			exit()
