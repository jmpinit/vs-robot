import math
import sys
import serial
import pygame

waypoints	= [(-1350, 20), (-625, -1060), (650, -1065), (1300, 30), (640, 1165), (-700, 1200)]
mines		= [(-1620, 320), (-1100, -1230), (510, -1575), (1600, -400), (1100, 1250), (-520, 1600)]
captures	= [(-1750, -375), (-550, -1700), (1200, -1300), (1700, 330), (600, 1750), (-1200, 1400)]

port = '/dev/rfcomm0'

if(len(sys.argv)==2):
	port = sys.argv[1]
else:
	print 'defaulting to '+port

screen = pygame.display.set_mode((1024, 600), pygame.FULLSCREEN)
ser = serial.Serial(port, 19200, timeout = 1)

path = list()
target = (512, 300)
heading = 0

def init():
	for i in range(0, len(waypoints)):
		waypoints[i] = bot_to_screen(waypoints[i][0], waypoints[i][1])
		mines[i] = bot_to_screen(mines[i][0], mines[i][1])
		captures[i] = bot_to_screen(captures[i][0], captures[i][1])

def bot_to_screen(x, y):
	newx = x/8.0+512
	newy = -y/8.0+300
	return (int(newx), int(newy))

def decode():
	line = ser.readline().strip()
	parts = line.split(" ")
	try:
		if(len(parts)>1):
			if(parts[0]=="p"):
				path.append(bot_to_screen(int(parts[1]), int(parts[2])))
			elif(parts[0]=="t"):
				target = bot_to_screen(int(parts[1]), int(parts[2]))
			elif(parts[0]=="h"):
				heading = float(parts[1])
	except:
		pygame.image.save(screen, "data.png");
		raise

def draw():
	screen.fill((0, 0, 0));

	# draw waypoints
	for pt in waypoints:
		pygame.draw.circle(screen, (0, 255, 0), pt, 16, 3);
		
	for pt in mines:
		pygame.draw.circle(screen, (0, 0, 255), pt, 16, 3);

	for pt in captures:
		pygame.draw.circle(screen, (255, 0, 0), pt, 16, 3);

	# draw path
	if(len(path)>1):
		pygame.draw.lines(screen, (255, 255, 255), False, path)

		# draw robot
		pygame.draw.circle(screen, (255, 255, 255), path[-1], 8)

		# draw heading
		pygame.draw.line(screen, (128, 128, 128), (path[-1][0], path[-1][1]), (path[-1][0]+12*math.cos(heading), path[-1][1]+12*math.sin(heading)), 2)

	pygame.draw.circle(screen, (255, 0, 0), target, 8)

	pygame.display.flip()

init()
while 1:
	decode()
	draw()

	for event in pygame.event.get():
		if(event.type==pygame.KEYDOWN):
			pygame.image.save(screen, "data.png");
			exit()
