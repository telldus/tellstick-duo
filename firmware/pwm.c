
#include <htc.h>
#include "pwm.h"

//PWM variables
struct pwms {
  char duty;
  signed char dir;
  char pulses;
} txd  = {0,1,0},
  rxd  = {0,1,0};
//  idle = {0,1,0};

void initPwm() {

	T2CKPS1 = 1; //Prescaler: 1:16

	//Postscaler, does not affect PWM
	T2OUTPS3 = 0;
	T2OUTPS2 = 1;
	T2OUTPS1 = 0;
	T2OUTPS0 = 0;

	CCP1CON = 0;
	CCP2CON = 0;

	//Pwm mode
	//Single output: P2A modulated
	CCP2M3 = 1;
	CCP2M2 = 1;

	//Single output: P1A modulated
	CCP1M3 = 1;
	CCP1M2 = 1;

	//We skip the lsb so setting them low
	DC1B1 = 0;
	DC1B0 = 0;
	DC2B1 = 0;
	DC2B0 = 0;

	//Turn off the led
	CCPR1L=0x0;
	CCPR2L=0x0;

	TMR2IF = 0;
	PR2 = 0xFF;

		//Low priority interrupt
	TMR2IP = 0;
	TMR2IE = 1;
	TMR2ON = 1; // Turn on timer

}

void pwmUpdate() {
	if (rxd.pulses) {
		rxd.duty += rxd.dir;
		if (rxd.duty >= 0x7F) {
			rxd.dir = -1;
		} else if (rxd.duty == 0) {
			rxd.dir = 1;
			--rxd.pulses;
		}
		CCPR1L = rxd.duty;
	}
	if (txd.pulses) {
		txd.duty += txd.dir;
		if (txd.duty >= 0x7F) {
			txd.dir = -1;
		} else if (txd.duty == 0) {
			txd.dir = 1;
			--txd.pulses;
		}
		CCPR2L = txd.duty;
	}
/*	if (rxd.pulses == 0 && txd.pulses == 0) {
		++idle.pulses;
		if (idle.pulses > 20) {
			idle.pulses = 0;
			idle.duty += idle.dir;
			if (idle.duty >= 0x1F) {
				idle.dir = -1;
			} else if (idle.duty == 0) {
				idle.dir = 1;
			}
			CCPR1L = idle.duty;
			CCPR2L = idle.duty;
		}
	} else {*/
		//These if-blocks is not in the above two ifblocks where it might seem more appropriate.
		//This is an optimation since it is not run when in "idle-mode". Which is most of the time
		if (rxd.pulses == 0) {
			CCPR1L = 0x0;
		}
		if (txd.pulses == 0) {
			CCPR2L = 0x0;
		}
//	}
}

void setRXPulses(unsigned char count) {
	rxd.pulses = count;
}

void setTXPulses(unsigned char count) {
	txd.pulses = count;
}
