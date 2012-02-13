/******************************************************************************
 * Software UART example for MSP430.
 *
 * Stefan Wendler
 * sw@kaltpost.de
 * http://gpio.kaltpost.de
 *
 * Echos back each character received. Blinks green LED in main loop. Toggles
 * red LED on RX.
 *
 * Use /dev/ttyACM0 at 9600 Bauds (and 8,N,1).
 ******************************************************************************/

#include <msp430.h>

#include "uart.h"

void uart_rx_isr(unsigned char c) {
	uart_putc(c);
	P1OUT ^= BIT0;		// toggle P1.0 (red led)
}
 
/**
 * Main routine
 */
int main(void)
{
     WDTCTL = WDTPW + WDTHOLD; 	// Stop WDT

     DCOCTL  = 0x00;            // Set DCOCLK to 1MHz
     BCSCTL1 = CALBC1_1MHZ;
     DCOCTL  = CALDCO_1MHZ;

	 P1DIR  = BIT0 + BIT6; 		// P1.0 and P1.6 are the red+green LEDs	
	 P1OUT  = BIT0 + BIT6; 		// All LEDs off

     uart_init();

	 // register ISR called when data was received
     uart_set_rx_isr_ptr(uart_rx_isr);

     __enable_interrupt();

     uart_puts("\n\r***************\n\r");
     uart_puts("MSP430 softuart\n\r");
     uart_puts("***************\n\r\n\r");

 	 volatile unsigned long i;

     while(1) {

		P1OUT ^= BIT6; 			// Toggle P1.6 output (green LED) using exclusive-OR
		i = 50000;             	// Delay

		do (i--);				// busy waiting (bad)
		while (i != 0);
     } 
}

