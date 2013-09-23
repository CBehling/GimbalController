
import usb.core

class GimbalUSB:

    def __init__(self):
        self.SET_POS = 0
        self.GET_POS = 1
        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()

    def close(self):
        self.dev = None

    def set_pos(self, pan, tilt):
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
