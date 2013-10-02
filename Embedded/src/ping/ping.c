#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "timer.h"
#include "oc.h"
#include "pin.h"
#include "uart.h"
#include "usb.h"
#include <stdio.h>

#define True 1
#define False 0

#define GET_DIST 2

uint16_t dist = 0;
uint16_t dist_capture_flag = False;

void VendorRequests(void) {
    WORD temp;

    switch (USB_setup.bRequest) {
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
    init_pin();
    init_timer();
    init_oc();
    init_uart();

    uint16_t pingOn = True;
    //configure pins
    pin_digitalIn(&(D[3]));                     //to read the comparator ultrasonic sensor output
    oc_pwm(&oc3, &(D[2]), NULL, 40e3, 1 << 15); //to send a 40kHz transmitter pulse
    timer_setPeriod(&timer3, 0.02);             //to control the ping size
    timer_start(&timer3);


    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }
    while (True) {
        ServiceUSB();
        //determine when to send the ping
    	if (!pingOn && timer_read(&timer3) < 1 << 8){
    		pingOn = True;
            if (dist_capture_flag){
                dist = 0;
            }
            dist_capture_flag = True;
    		pin_write(&(D[2]), 1 << 15);
		}else if(pingOn && timer_read(&timer3) >= 1 << 8){
			pingOn = False;
			pin_write(&(D[2]), 0);
		}

        //determine when to read the ping
        if(timer_read(&timer3) >= 0b1101 << 7){
            if (pin_read(&D[3])) {
                if (dist_capture_flag){
                    dist = timer_read(&timer3);
                    dist_capture_flag = False;
                }
            }
        }
    }
}