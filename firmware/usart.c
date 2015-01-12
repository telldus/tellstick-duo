
#include <htc.h>
#include <stdio.h>
#include <string.h>
#include "config.h"
#include "pwm.h"
#include "usart.h"
#include "transmit.h"

#define TRANSMIT_BUFFER 256
#define RECEIVE_BUFFER 512

static volatile unsigned char buffer[RECEIVE_BUFFER];
static volatile unsigned char tbuffer[TRANSMIT_BUFFER];
static volatile unsigned short bufferP = 0;
volatile bit dataAvailable = 0;
static char timeouts = 0;

static volatile unsigned char tbufferP = 0, tbufferP2 = TRANSMIT_BUFFER-1;

void handleMessage();

void usartTXUpdate() {
	unsigned char p = tbufferP2;
	if (p >= TRANSMIT_BUFFER-1) {
		p = 0;
	} else {
		++p;
	}
	if (p == tbufferP) {
		//Nothing to transmit
		TXIE = 0;
		return;
	}
	
	TXREG = tbuffer[p];
	tbufferP2 = p;
}

void putch( char ch ) {
	//Wait until we have room in the buffer
	while(tbufferP == tbufferP2);
	
	tbuffer[tbufferP] = ch;
	if (tbufferP >= TRANSMIT_BUFFER-1) {
		tbufferP = 0;
	} else {
		++tbufferP;
	}
	TXIE=1;
}

char usartReady() {
	return PIR1bits.RCIF;
}

unsigned char usartGetByte() {
	unsigned char rslt = RCREG;
	
	if (RCSTAbits.OERR) {
		RCSTAbits.CREN = 0;
		RCSTAbits.CREN = 1;
	}
	return rslt;
}

void initUsart() {
	unsigned short i;

	RCSTA = 0;
	RCSTAbits.SPEN = 1;  //Enable Usart
	RCSTAbits.CREN = 1;  //Enable reciever

	TXSTAbits.TX9D = 0;  //8-bit transmission
	TXSTAbits.TXEN = 1;  //Transmit enabled
	TXSTAbits.BRGH = 0;  //High speed
	BRG16 = 0;

	TRISC7 = 1;
	TRISC6 = 0;

	while (PIR1bits.RCIF) {
		i = RCREG;
	}

	SPBRG = 64;
	
	//Low priority
	RCIP = 0;
	TXIP = 0;
	
	//Enable interrupts
	PEIE = 1;
	RCIE = 1;

	//No interrupt on transmit, our transmission-code handles this
	TXIE = 0;
	
	//Init timeout timer
	TMR1ON = 0;
	TMR1IF = 0;
	TMR1H = 0x0;
	TMR1L = 0x0;
	
	T1CKPS1 = 0;
	T1CKPS0 = 0;
	TMR1CS = 0; //Clock source = internal clock (Fosc/4)

	memset(buffer, 0, sizeof(buffer));
	memset(tbuffer, '-', sizeof(tbuffer));
}

void usartRCUpdate() {
	unsigned char rslt = RCREG;
	
	if (RCSTAbits.OERR) {
		RCSTAbits.CREN = 0;
		RCSTAbits.CREN = 1;
	}
	
	if (dataAvailable) {
		//Still parsing an old message
		return;
	}
	
	if (rslt == '+') {
		TMR1ON = 0;
		dataAvailable = 1;
		buffer[bufferP] = 0;
	} else {
		buffer[bufferP] = rslt;
		++bufferP;
		if (bufferP >= RECEIVE_BUFFER) {
			buffer[bufferP-1] = 0;
			bufferP = 0;
		}
		//Start timeout timer
		TMR1H = 0x0;
		TMR1L = 0x0;
		TMR1ON = 1;
		TMR1IF = 0;
		timeouts = 0;
	}
}

void usartTask() {
	if (TMR1IF) {
		if (timeouts > 30) {
			TMR1ON = 0;
		
			bufferP = 0;
			dataAvailable = 0;
		} else {
			++timeouts;
		}
		TMR1IF = 0;
	}
	if (!dataAvailable) {
		return;
	}
	
	handleMessage();

	memset(buffer, 0, sizeof(buffer));
	bufferP = 0;
	dataAvailable = 0;
}

void send(unsigned short start, unsigned char pause, unsigned char repeats) {
	rfStartTransmit();
	for(unsigned char i = 0; i < repeats; ++i) {
		setTXPulses(2);
		rfSend(&buffer[start]);
		for(unsigned char j = 0; j < pause; ++j) {
			__delay_ms(1);
		}
	}
	rfStopTransmit();
}

void sendExtended(unsigned short start, unsigned char pause, unsigned char repeats) {
	rfStartTransmit();
	for(unsigned char i = 0; i < repeats; ++i) {
		setTXPulses(2);
		rfSendExtended(&buffer[start]);
		for(unsigned char j = 0; j < pause; ++j) {
			__delay_ms(1);
		}
	}
	rfStopTransmit();
}

void handleMessage() {
	unsigned char pause = 11;
	unsigned char repeats = 10;
	
	if (strncmp(buffer, "V", 1) == 0) {
		printf("+V%u\r\n", FIRMWARE_VERSION);
		return;
	} else if (strncmp(buffer, "N", 1) == 0) {
		printf("+N\r\n");
		return;
	}
	unsigned char p = 0, found = 0;
	while(!found) {
		switch (buffer[p]) {
			case 'P':
				pause = buffer[p+1];
				++p;
				break;
			case 'R':
				repeats = buffer[p+1];
				++p;
				break;
			case 'S':
				send(p+1, pause, repeats);
				found = 1;
				printf("+S\r\n");
				break;
			case 'T':
				sendExtended(p+1, pause, repeats);
				found = 1;
				printf("+T\r\n");
				break;
			default:
				printf("Unknown command (%u)\r\n", buffer[p]);
				return;
		}
		++p;
	}
}
