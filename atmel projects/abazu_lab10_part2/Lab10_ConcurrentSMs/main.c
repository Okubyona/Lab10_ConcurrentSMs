
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef enum States {bInit, On, Off} States;

typedef enum SeqStates {seqInit, seq0, seq1, seq2} SeqStates;

typedef enum CombineStates {combine} CombineStates;

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
unsigned char tmpA;
static unsigned char tmpB;

int main(void) {
	DDRB = 0xFF; PORTB = 0x00;



	States state = bInit;
	SeqStates sState = seqInit;
	CombineStates cState = combine;
	const unsigned short timerPeriod = 50;
	unsigned short bElapsedTime = 1000;
	unsigned short thElapsedTime = 300;

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
		//PORTB = tmpB | tmpA;
		cState = combineSM(cState);
		while(!TimerFlag) {}
		TimerFlag = 0;
		bElapsedTime += timerPeriod;
		thElapsedTime += timerPeriod;
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

int combineSM(int state) {
	switch (state) {
		case combine:
		state = combine;
		PORTB = tmpB | tmpA;
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
