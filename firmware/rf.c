#include "receive.h"
#include "config.h"

void activateReceiver() {
	TX_ANT = 0;
	RX_ANT = 1;
}

void activateTransmitter() {
	TX_ANT = 1;
	RX_ANT = 0;
}

void initReceiver() {
	T0PS2 = 0;
	T0PS1 = 0;
	T0PS0 = 0;
	
	PSA = 0; // Assign prescaler
	
	T0CS = 0; // Clock source = CLKO
	
	T08BIT = 1; // Timer0 8-bit mode
	
	TMR0IF = 0; // Clear timer0 flag
	TMR0IE  = 1; // Enable interrupt on timer0


	//SPDT control
	RX_ANT_TRIS = 0; //Output
	
	//The ANT is an input
	RECEIVER_TRIS = 1;

	//High priority interrupt
	TMR0IP = 1;

	// Turn on timer
	TMR0ON = 1;

	activateReceiver();
}

void initTransmitter() {
	//SPDT control
	TX_ANT_TRIS = 0; //Output
	
	//The SENDER is an output
	SENDER_TRIS = 0;
	
	SENDER = 0;
	
	//Timer 3
	
	T3CKPS1 = 0;
	T3CKPS0 = 0;
	
	TMR3IE = 1;
	TMR3IP = 1;
	TMR3ON = 0;

	
}

void rfStartTransmit() {
	TMR0ON=0; //Turn off receive sampling
	activateTransmitter();
}

void rfStopTransmit() {
	activateReceiver();
	TMR0ON = 1; //Turn on receive sampling
	TMR3ON = 0;
}
