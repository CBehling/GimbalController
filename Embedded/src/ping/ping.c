#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "timer.h"
#include "oc.h"
#include "pin.h"

#define True 1
#define False 0

int16_t main(void) {
    //initialize modules
    init_clock();
    init_pin();
    init_timer();
    init_oc();

    uint16_t pingOn = True;
    //configure pins
    oc_pwm(&oc3, &(D[2]), NULL, 40e3, 1 << 15);
    timer_setPeriod(&timer3, 0.02);
    timer_start(&timer3);

    while (1) {
    	if (!pingOn && timer_read(&timer3) < 1 << 10){
    		pingOn = True;
    		pin_write(&(D[2]), 1 << 15);
		}else if(pingOn && timer_read(&timer3) >= 1 << 10){
			pingOn = False;
			pin_write(&(D[2]), 0);
		}
    }
}

