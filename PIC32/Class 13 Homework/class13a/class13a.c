// If the USER button is pressed, LED1 is on and LED2 is off,
// else LED1 is off and LED2 is on.

#include "NU32.h"          // constants, funcs for startup and UART
#include <stdio.h>

void checkUser(void);

int main(void) {
  NU32_Startup();
  TRISF = 0xFFFC;        // Pins 0 and 1 of Port F are LED1 and LED2.  Clear
                         // bits 0 and 1 to zero, for output.  Others are inputs.
  NU32_LED1 = 0;         // Turn LED1 on.
  NU32_LED2 = 1;         // Turn LED2 off.

  while(1) {
    checkUser();         // Function that checks whether the USER button is pressed,
                         // and if it's pressed LED1 turns off and LED2 turns on.
    NU32_LED1 = 0;
    NU32_LED2 = 1;
  }
  return 0;
}

void checkUser(void) {

  while(!PORTDbits.RD7) {
    NU32_LED1 = 1;
    NU32_LED2 = 0;
  }
}
