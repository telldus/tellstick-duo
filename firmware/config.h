#include <htc.h>

#define FIRMWARE_VERSION 5

#define RECEIVER         RB4
#define RECEIVER_TRIS    TRISB4

#define SENDER           (LATA4)
#define SENDER_TRIS      (TRISA4)

#define TX_ANT_TRIS      (TRISA2)
#define TX_ANT           (LATA2)
#define RX_ANT_TRIS      (TRISA3)
#define RX_ANT           (LATA3)

#define LED0             RC1
#define LED0_TRIS        TRISC1
#define LED1             RC2
#define LED1_TRIS        TRISC2

#define DEBUG_LED        RA3
#define DEBUG_LED_TRIS   TRISA3

#define SPECIAL_TRIS     TRISB2

#define TMR0_RELOAD      175

#define TIME_MATCH       5

//#define DATA_BYTES       512

#define _XTAL_FREQ       40000000
