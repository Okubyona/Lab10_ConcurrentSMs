/*	Author: abazu001
 *  Partner(s) Name:
 *	Lab Section:
 *	Assignment: Lab #10  Exercise #3
 *	Exercise Description: [To the previous exercise's implementation, connect
    your speaker's red wire to PB4 and black wire to ground. Add a third task
    that toggles PB4 on for 2 ms and off for 2 ms as long as a switch on PA2 is
    in the on position. Don’t use the PWM for this task.
]
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *  Demo Link:
 *  https://drive.google.com/file/d/14kovbLSFXhhMZ5Ey7NuKsXMjG8rTAr5L/view?usp=sharing
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef enum States {bInit, On, Off} States;
typedef enum SeqStates {seqInit, seq0, seq1, seq2} SeqStates;
typedef enum CombineStates {combine} CombineStates;
typedef enum SpeakerStates {low, high} SpeakerStates;

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0;   // Current internal count of 1 ms ticks.

void TimerOn();
void TimerOff();
void TimerSet(unsigned long);
void TimerISR();
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--;   // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) {
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

int blinkLED(int state);
int threeLED(int state);
int combineSM(int state);
int speaker(int state);
unsigned char tmpA;
unsigned char tmpB;
unsigned char tmpC;

int main(void) {
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;



	States state = bInit;
	SeqStates sState = seqInit;
	CombineStates cState = combine;
	SpeakerStates speakerState = low;
	const unsigned char timerPeriod = 1;
	unsigned short bElapsedTime = 1000;
	unsigned short thElapsedTime = 300;
	unsigned short speakerElapsedTime = 2;

	TimerOn();
	TimerSet(timerPeriod);

	while(1) {
		if (bElapsedTime >= 1000) {
			state = blinkLED(state);
			bElapsedTime = 0;
		}

		if (thElapsedTime >= 300) {
			sState = threeLED(sState);
			thElapsedTime = 0;
		}

		if (speakerElapsedTime >= 2) {
			speakerState = speaker(speakerState);
			speakerElapsedTime = 0;
		}
		//PORTB = tmpB | tmpA;
		cState = combineSM(cState);
		while(!TimerFlag) {}
		TimerFlag = 0;
		bElapsedTime += timerPeriod;
		thElapsedTime += timerPeriod;
		speakerElapsedTime += timerPeriod;
	}

	return 0;
}

int blinkLED (int state) {

	switch (state) {
		case bInit:
		state = Off;
		break;

		case On:
		state = Off;
		break;

		case Off:
		state = On;
		break;
	}

	switch (state) {
		case bInit: break;

		case On:
		tmpB = 0x08;
		break;

		case Off:
		tmpB = 0x00;
		break;
	}


	return state;
}

int threeLED(int state) {

	switch (state) {
		case seqInit:
		state = seq0;
		break;

		case seq0:
		state = seq1;
		break;

		case seq1:
		state = seq2;
		break;

		case seq2:
		state = seq0;
		break;

	}

	switch (state) {
		case seqInit: break;

		case seq0:
		tmpA = 0x01;
		break;

		case seq1:
		tmpA = 0x02;
		break;

		case seq2:
		tmpA = 0x04;
		break;
	}

	return state;
}

int speaker(int state) {
	unsigned char A2 = ~PINA & 0x04;

	switch (state) {
		case low:
		state = A2 ? high:low;
		break;

		case high:
		state = low;
		break;
	}

	switch (state) {
		case low:
		tmpC = 0x00;
		break;

		case high:
		tmpC = 0x10;
		break;
	}

	return state;
}

int combineSM(int state) {
	switch (state) {
		case combine:
		state = combine;
		PORTB = (tmpB | tmpA | tmpC);
		break;
	}
	return state;
}


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


void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
