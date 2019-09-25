// Edit the code from Chapter 6.2 (external interrupt that blinks the LEDs) to
// also print "Pressed x times!" when the interrupt occurs, where x is a count
// of the number of times the USER button has been pressed.

#include "NU32.h"          // constants, funcs for startup and UART
#include <stdio.h>

volatile int COUNT;

void __ISR(_EXTERNAL_0_VECTOR, IPL2SOFT) Ext0ISR(void) { // step 1: the ISR
  char message[20];
  LATFINV = 0x0003;   //  turn LEDs ON
  _CP0_SET_COUNT(0);

  while(_CP0_GET_COUNT() < 20000000) { ; } // delay for 20 M core ticks, 0.5 s

  LATFINV = 0x0003;   //  turn LEDs off
  COUNT++;
  sprintf(message,"Pressed %d times!\r\n", COUNT);
  NU32_WriteUART3(message);
  IFS0bits.INT0IF = 0;            // clear interrupt flag IFS0<3>
}

int main(void) {
  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32_LED1 = 1;
  NU32_LED2 = 1;
  __builtin_disable_interrupts(); // step 2: disable interrupts
  INTCONbits.INT0EP = 0;          // step 3: INT0 triggers on falling edge
  IPC0bits.INT0IP = 2;            // step 4: interrupt priority 2
  IPC0bits.INT0IS = 1;            // step 4: interrupt priority 1
  IFS0bits.INT0IF = 0;            // step 5: clear the int flag
  IEC0bits.INT0IE = 1;            // step 6: enable INT0 by setting IEC0<3>
  __builtin_enable_interrupts();  // step 7: enable interrupts
                                  // Connect RD7 (USER button) to INT0 (RD0)
  while(1) {
      ; // do nothing, loop forever
  }

  return 0;
}
