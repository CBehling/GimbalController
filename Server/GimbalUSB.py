import usb.core
from time import sleep
from math import pi as pi

class GimbalUSB:

    def __init__(self):
        self.SET_POS = 0
        self.GET_POS = 1
        self.GET_DIST = 2
        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()

    def close(self):
        self.dev = None

    def set_pos(self, pan, tilt):
        pan = pan*65535/pi;
        tilt = tilt*65535/pi;
        print 'setting pan/tilt'
        try:
            self.dev.ctrl_transfer(0x40, self.SET_POS, int(pan), int(tilt))
        except usb.core.USBError:
            print "Could not send SET_POS vendor request."

    def get_pos(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.GET_POS, 0, 0, 4)
        except usb.core.USBError:
            print "Could not send GET_POS vendor request."
        else:
            return [int(ret[0])+int(ret[1])*256, int(ret[2])+int(ret[3])*256]

    def get_dist(self):
        try:
            ret = self.dev.ctrl_transfer(0xC0, self.GET_DIST, 0, 0, 2)
        except usb.core.USBError:
            print "Could not send GET_DIST vendor request."
        else:
            return int(ret[0])+int(ret[1])*256

    def scan(self):
        for pan in range(0,65535,4096):
            for tilt in range(0,65535,4096):
                self.set_pos(pan,tilt)
                sleep(.1)
