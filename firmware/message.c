#include "message.h"
#include "pwm.h"
#include <htc.h>
#include <stdio.h>

void rfMessageBegin() {
	printf("+R");
}

void rfMessageBeginRaw() {
	printf("+W");
}

void rfMessageEnd(unsigned char type) {
	printf("\r\n");
	setRXPulses(type);
}

void rfMessageAddByte(const char *key, unsigned char value) {
	printf("%s:0x%X;", key, value);
}

void rfMessageAddLong(const char *key, unsigned long value) {
	printf("%s:0x", key);

	if (value == 0) {
		printf("0");
	} else {
		unsigned char first = 1;
		for(unsigned char i = 0; i < 2*sizeof(long); ++i) {
			//Mask (and shift) out the highest nibble
			unsigned char nibble = (unsigned char)(value >> (8*sizeof(long)-4)) & 0xF;

			if (nibble == 0 && first == 1) {
				//Don't send prepending zeroes
			} else {
				printf("%X", nibble);
				first = 0;
			}
			value <<= 4;
		}
	}
	printf(";");
}

void rfMessageAddString(const char *key, const char *value) {
	printf("%s:%s;", key, value);
}

void rfMessageAddHexString(const char *key, const char *value, const unsigned char length) {
	printf("%s:", key);
	for(unsigned char i = 0; i < length; ++i) {
		printf("%X%X", value[i]>>4, value[i]&0xF);
	}
	printf(";");
}
