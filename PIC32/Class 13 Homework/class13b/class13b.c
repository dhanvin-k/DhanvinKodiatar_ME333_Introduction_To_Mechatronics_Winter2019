// If the USER button goes from HIGH to LOW, turn on LED1 and LED2 for 0.5s
// and print "Pressed x times!", else LED1 and LED2 are off, where x is a count
// of the number of times the USER button has been pressed.

#include "NU32.h"          // constants, funcs for startup and UART
#include <stdio.h>

int main(void) {
  NU32_Startup();
  unsigned int x = 0;
  char message[20];
  NU32_LED1 = 1;
  NU32_LED2 = 1;
  while (1) {
    if (!PORTDbits.RD7) {
      LATFINV = 0x0003;     // Turn LEDs ON
      _CP0_SET_COUNT(0);    // Restart clock to 0
      while (_CP0_GET_COUNT()<20000000) { ; } // delay for 20 M core ticks, 0.5 s
      LATFINV = 0x0003;     // Turn LEDs off
      x++;
      sprintf(message,"Pressed %d times!\r\n", x);
      NU32_WriteUART3(message);
    }
  }
  return 0;
}
