
#include <htc.h>
#include "config.h"
#include "usart.h"
#include "pwm.h"
#include "rf.h"
#include "receive.h"
#include "transmit.h"

#include <stdio.h>

__CONFIG(1, HSPLL);
__CONFIG(2, WDTDIS);
__CONFIG(4, XINSTDIS);

void interrupt HighISR(void) {
	if (TMR0IF) {
		rfReceiveUpdate(RECEIVER);
		//Reset timer
		TMR0L = 0;
		TMR0IF = 0;
	}
	if (TMR3IF) {
		TMR3H = 0xFF;
		TMR3L = 0xC9;
		rfTransmitUpdate();
		TMR3IF = 0;
	}
}

void interrupt low_priority LowISR(void) {
	if (RCIF) {
		usartRCUpdate();
	}
	if (TXIF) {
		usartTXUpdate();
	}
	if (TMR2IF) {
		pwmUpdate();
		TMR2IF = 0;
	}
}

void initInterrupts() {
	//Enable interrupt priority
	RCONbits.IPEN = 1;

	//Enable interrupts
	GIEH = 1; //High priority
	GIEL = 1; //Low priority
	
	GIE = 1;
}

void initPort() {
	ADCON0 = 0x00;
	ADCON1 = 0x0F;             // All inputs as digital
	CMCON  = 0x07;             // turn off comparators
	PORTA = 0;                 // Initialize PORTA
	TRISA = 0x0;               // Configure PORTA as output
	TRISC = 0;
	TRISD = 0;
	PORTC = 0;
	PORTD = 0;
	
	TRISB = 0x00;              // Port B output
	PORTB = 0;
	
	SENDER = 0;
	SENDER_TRIS = 0;           // The SENDER is an output
	
	LED0_TRIS = 0;
	LED1_TRIS = 0;
	DEBUG_LED_TRIS = 0;
}

int main() {
	initPort();
	initInterrupts();
	initUsart();
	initReceiver();
	initTransmitter();
	initPwm();
	
	printf("\r\n+V%u\r\n", FIRMWARE_VERSION);

	while(1) {
		rfReceiveTask();
		usartTask();
	}
	return 42;
}
