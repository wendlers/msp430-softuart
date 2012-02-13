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

#include "uart.h"

/**
 * Main routine
 */
int main(void)
{
     WDTCTL = WDTPW + WDTHOLD; 	// Stop WDT

     DCOCTL  = 0x00;            // Set DCOCLK to 1MHz
     BCSCTL1 = CALBC1_1MHZ;
     DCOCTL  = CALDCO_1MHZ;

     uart_init();

     __enable_interrupt();

     uart_puts("\n\r***************\n\r");
     uart_puts("MSP430 softuart\n\r");
     uart_puts("***************\n\r\n\r");

     unsigned char c;

     while(1) {
          c = uart_getc();
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

