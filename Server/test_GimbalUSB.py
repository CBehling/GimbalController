from GimbalUSB import GimbalUSB
from math import pi as pi
a = GimbalUSB()
a.set_pos(pi/2,pi/2)
while True:
	print a.get_dist()