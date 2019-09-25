#include "NU32.h"          // constants, functions for startup and UART
#include <stdio.h>

int main(void) {
  NU32_Startup();          // cache on, interrupts on, LED/button init, UART init

  T3CONbits.TCKPS = 0;     // Timer3 prescaler N=1 (1:1)
  PR3 = 3999;              // period = (PR3+1) * N * 12.5 ns = 50 us, 20 kHz
  TMR3 = 0;                // initial TMR2 count is 0
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC1CONbits.OCTSEL = 1;
  OC1RS = 3000;             // duty cycle = OC1RS/(PR2+1) = 75%
  OC1R = 3000;              // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;        // turn on Timer3
  OC1CONbits.ON = 1;       // turn on OC1

  while(1) { ; }      // infinite loop
  return 0;
}
