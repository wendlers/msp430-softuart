/******************************************************************************
 * Software UART example for MSP430.
 *
 * Stefan Wendler
 * sw@kaltpost.de
 * http://gpio.kaltpost.de
 *
 * Echos back each character received enclosed in square brackets "[" and "]".
 * Use /dev/ttyACM0 at 9600 Bauds (and 8,N,1).
 ******************************************************************************/

#include <msp430.h>
#include <stdint.h>

#include "uart.h"

/**
 * Main routine
 */
int main(void)
{
     WDTCTL = WDTPW + WDTHOLD; 	// Stop WDT

     BCSCTL1 = CALBC1_1MHZ; 		// Set range
     DCOCTL = CALDCO_1MHZ; 		// SMCLK = DCO = 1MHz

     uart_init();

     __enable_interrupt();

     uart_puts("\n\r***************\n\r");
     uart_puts("MSP430 softuart\n\r");
     uart_puts("***************\n\r\n\r");

     uint8_t c;

     while(1) {
          if(uart_getc(&c)) {
               if(c == '\r') {
                    uart_putc('\n');
                    uart_putc('\r');
               } else {
                    uart_putc('[');
                    uart_putc(c);
                    uart_putc(']');
               }
          }
     }
}

