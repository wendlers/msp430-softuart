/******************************************************************************
 * Software UART example for MSP430.
 *
 * Stefan Wendler
 * sw@kaltpost.de
 * http://gpio.kaltpost.de
 *
 ******************************************************************************
 * Code is based on TIs Timer_A UART example
 ******************************************************************************/

#include <msp430.h>
#include <legacymsp430.h>

#include "uart.h"

#define UART_TBIT_DIV_2     (1000000 / (9600 * 2))
#define UART_TBIT           (1000000 / 9600)

unsigned int  txData;                       // UART internal variable for TX
unsigned char rxBuffer;                     // Received UART character

void uart_init(void)
{
	P1SEL = UART_TXD + UART_RXD;            // Timer function for TXD/RXD pins
    P1DIR |= UART_TXD;						// Set TXD to output
	TACCTL0 = OUT;                          // Set TXD Idle as Mark = '1'
    TACCTL1 = SCS + CM1 + CAP + CCIE;       // Sync, Neg Edge, Capture, Int
    TACTL   = TASSEL_2 + MC_2;              // SMCLK, start in continuous mode
}

unsigned char uart_getc()
{
	// wait for incomming data
	__bis_SR_register(LPM0_bits);
	return rxBuffer;
}

void uart_putc(unsigned char c)
{
     while (TACCTL0 & CCIE);                 // Ensure last char got TX'd
     TACCR0 = TAR;                           // Current state of TA counter
     TACCR0 += UART_TBIT;                    // One bit time till first bit
     TACCTL0 = OUTMOD0 + CCIE;               // Set TXD on EQU0, Int
     txData = c;                             // Load global variable
     txData |= 0x100;                        // Add mark stop bit to TXData
     txData <<= 1;                           // Add space start bit
}

void uart_puts(const char *str)
{
     while(*str) uart_putc(*str++);
}

interrupt(TIMERA0_VECTOR) Timer_A0_ISR(void)
{
    static unsigned char txBitCnt = 10;
 
    TACCR0 += UART_TBIT;                    // Add Offset to CCRx
    if (txBitCnt == 0) {                    // All bits TXed?
        TACCTL0 &= ~CCIE;                   // All bits TXed, disable interrupt
        txBitCnt = 10;                      // Re-load bit counter
    }
    else {
        if (txData & 0x01) {
          TACCTL0 &= ~OUTMOD2;              // TX Mark '1'
        }
        else {
          TACCTL0 |= OUTMOD2;               // TX Space '0'
        }
        txData >>= 1;
        txBitCnt--;
    }
}      

interrupt(TIMERA1_VECTOR) Timer_A1_ISR(void)
{
    static unsigned char rxBitCnt = 8;
    static unsigned char rxData   = 0;
 
    switch (TAIV) { 							 // Use calculated branching
        case TAIV_TACCR1:                        // TACCR1 CCIFG - UART RX
            TACCR1 += UART_TBIT;                 // Add Offset to CCRx
            if (TACCTL1 & CAP) {                 // Capture mode = start bit edge
                TACCTL1 &= ~CAP;                 // Switch capture to compare mode
                TACCR1 += UART_TBIT_DIV_2;       // Point CCRx to middle of D0
            }
            else {
                rxData >>= 1;
                if (TACCTL1 & SCCI) {            // Get bit waiting in receive latch
                    rxData |= 0x80;
                }
                rxBitCnt--;
                if (rxBitCnt == 0) {             // All bits RXed?
                    rxBuffer = rxData;           // Store in global variable
                    rxBitCnt = 8;                // Re-load bit counter
                    TACCTL1 |= CAP;              // Switch compare to capture mode
                    __bic_SR_register_on_exit(LPM0_bits);  // Clear LPM0 bits from 0(SR)
                }
            }
            break;
    }
}
