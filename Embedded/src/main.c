#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "usb.h"
#include "pin.h"
#include "uart.h"
#include "oc.h"
#include "timer.h"
#include <stdio.h>

#define SET_POS     0
#define GET_POS     1

uint16_t pan, tilt;

//void ClassRequests(void) {
//    switch (USB_setup.bRequest) {
//        default:
//            USB_error_flags |= 0x01;                    // set Request Error Flag
//    }
//}

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
    pan = tilt = 0;

    //configure pins
    pin_digitalOut(&(D[0]));
    pin_digitalOut(&(D[1]));
    oc_servo(&oc1, &(D[0]), &timer1, 20e-3, 0.600e-3, 2.5e-3, 0);
    oc_servo(&oc2, &(D[1]), &timer2, 20e-3, 0.575e-3, 2.5e-3, 0);

    InitUSB();                              // initialize the USB registers and serial interface engine
    while (USB_USWSTAT!=CONFIG_STATE) {     // while the peripheral is not configured...
        ServiceUSB();                       // ...service USB requests
    }
    while (1) {
        ServiceUSB();                       // service any pending USB requests
    }
}

