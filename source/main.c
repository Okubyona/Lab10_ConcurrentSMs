/*	Author: abazu001
 *  Partner(s) Name:
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #1
 *	Exercise Description: [ Connect LEDs to PB0, PB1, PB2, and PB3.
        * In one state machine (ThreeLEDsSM), output to a shared variable (threeLEDs)
        the following behavior: set only bit 0 to 1, then only bit 1, then only
        bit 2 in sequence for 1 second each.
        * In a second state machine (BlinkingLEDSM), output to a shared variable
        (blinkingLED) the following behavior: set bit 3 to 1 for 1 second, then
        0 for 1 second.
        * In a third state machine (CombineLEDsSM), combine both shared variables 
        and output to the PORTB ]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef enum States {init, wait, } States;

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0;   // Current internal count of 1 ms ticks.

void TimerOn() {
    TCCR1B = 0x0B;

    OCR1A = 125;

    TIMSK1 = 0x02;

    TCNT1=0;

    _avr_timer_cntcurr = _avr_timer_M;

    SREG |= 0x80;

}

void TimerOff() {
    TCCR1B = 0x00;
}

void TimerISR() {
    TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
    // CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
    _avr_timer_cntcurr--;   // Count down to 0 rather than up to TOP
    if (_avr_timer_cntcurr == 0) {
        TimerISR();
        _avr_timer_cntcurr = _avr_timer_M;
    }

}

void TimerSet(unsigned long M) {
    _avr_timer_M = M;
    _avr_timer_cntcurr = _avr_timer_M;
}

int Tick(int state) {

    switch (state) {
        case init:


        case wait:

            break;

        default:
            state = init;
            break;
    }

    switch (state) {
        case init:
            break;

        case wait: break;


    }

    return state;
}

int main(void) {

	TimerOn();
	TimerSet(250);

    States state = init;

    /* Insert your solution below */
    while (1) {
        state = Tick(state);

        while(!TimerFlag) {}
        TimerFlag = 0;
    }
    return 1;
}
