#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "usb.h"
#include "pin.h"
#include "uart.h"
#include "oc.h"
#include "timer.h"
#include <stdio.h>


#define True    1
#define False   0

#define SET_POS     0
#define GET_POS     1
#define GET_DIST    2

uint16_t pan, tilt, dist;

void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
        case SET_POS:
            pan = USB_setup.wValue.w;
            tilt = USB_setup.wIndex.w;
            BD[EP0IN].bytecount = 0;    // set EP0 IN byte count to 0 
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            pin_write(&(D[1]), pan);
            pin_write(&(D[0]), tilt);
            break;
        case GET_POS:
            temp.w = pan;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            temp.w = tilt;
            BD[EP0IN].address[2] = temp.b[0];
            BD[EP0IN].address[3] = temp.b[1];
            BD[EP0IN].bytecount = 4;    // set EP0 IN byte count to 4
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        case GET_DIST:
            temp.w = dist;
            BD[EP0IN].address[0] = temp.b[0];
            BD[EP0IN].address[1] = temp.b[1];
            BD[EP0IN].bytecount = 2;    // set EP0 IN byte count to 4
            BD[EP0IN].status = 0xC8;    // send packet as DATA1, set UOWN bit
            break;
        default:
            USB_error_flags |= 0x01;    // set Request Error Flag
    }
}

void VendorRequestsIn(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

void VendorRequestsOut(void) {
    switch (USB_request.setup.bRequest) {
        default:
            USB_error_flags |= 0x01;                    // set Request Error Flag
    }
}

int16_t main(void) {
    //initialize modules
    init_clock();
    init_uart();
    init_pin();;
    init_timer();
    init_oc();

    //initialize values
    pan = tilt = dist = 0;

    uint16_t distCapt = True;
    uint16_t pingOn = True;

    //configure pins
    //pin_digitalOut(&(D[0]));  //probably don't need this
    //pin_digitalOut(&(D[1]));  //probably don't need this
    oc_servo(&oc1, &(D[0]), &timer1, 20e-3, 0.600e-3, 2.5e-3, 0);   //to control the tilt servo
    oc_servo(&oc2, &(D[1]), &timer2, 20e-3, 0.575e-3, 2.5e-3, 0);   //to control the pan servo
    pin_digitalIn(&(D[3]));                     //to read the comparator ultrasonic sensor output
    oc_pwm(&oc3, &(D[2]), NULL, 40e3, 1 << 15); //to send a 40kHz transmitter pulse
    timer_setPeriod(&timer3, 0.02);             //to control the ping size
    timer_start(&timer3);

    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }
    while (1) {
        ServiceUSB();                       // service any pending USB requests
        //determine when to send the ping
        if (!pingOn && timer_read(&timer3) < 1 << 8){
            pingOn = True;
            if (distCapt){
                dist = 0;
            }
            distCapt = True;
            pin_write(&(D[2]), 1 << 15);
        }else if(pingOn && timer_read(&timer3) >= 1 << 8){
            pingOn = False;
            pin_write(&(D[2]), 0);
        }

        //determine when to read the ping
        if(timer_read(&timer3) >= 0b1101 << 7 && pin_read(&D[3]) && distCapt){
//            if (pin_read(&D[3])) {
//                if (distCapt){
                    dist = timer_read(&timer3);
                    distCapt = False;
//                }
//            }
        }
    }
}

