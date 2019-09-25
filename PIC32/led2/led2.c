#include "NU32.h"          // constants, functions for startup and UART
#include <stdio.h>

#define  NUMSAMPS 1000      // number of points in waveform
static volatile int Waveform[NUMSAMPS];     // waveform

void makeWaveform() {
  int i = 0, center = 2000, A = 1000;     // square wave, fill in center value and amplitude
  for (i = 0; i < NUMSAMPS; ++i) {
    if ( i<NUMSAMPS/2 ) {
      Waveform[i] = center + A;
    } else {
      Waveform[i] = center - A;
    }
  }
}

void __ISR (_TIMER_2_VECTOR, IPL5SOFT) Controller (void){     // _TIMER_2_VECTOR = 8
  static int counter = 0;     // initialize counter once

  OC1RS = Waveform[counter];

  counter++;      // add one to counter every time ISR is entered
  if (counter == NUMSAMPS) {
    counter = 0;      // Roll the counter over when needed
  }
  IFS0bits.T2IF = 0;      // clearing the interrupt flag
}

int main(void) {
  NU32_Startup();          // cache on, interrupts on, LED/button init, UART init

  makeWaveform();
  T3CONbits.TCKPS = 0;     // Timer3 prescaler N=1 (1:1)
  PR3 = 3999;              // period = (PR3+1) * N * 12.5 ns = 50 us, 20 kHz
  TMR3 = 0;                // initial TMR3 count is 0
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC1CONbits.OCTSEL = 1;
  OC1RS = 3000;             // duty cycle = OC1RS/(PR3+1) = 75%
  OC1R = 3000;              // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;        // turn on Timer3
  OC1CONbits.ON = 1;       // turn on OC1

  __builtin_disable_interrupts();     // step 2: disable interrupts at CPU
                          // step 3: setup peripheral
  T2CONbits.TCKPS = 1;    //         set prescaler to 2
  PR2 = 39999;            //         set period register
  TMR2 = 0;               //         initialize count to 0
  T2CONbits.ON = 1;       //         Turning on timer2
  IPC2bits.T2IP = 5;      // step 4: priority
  IPC2bits.T2IS = 0;
  IFS0bits.T2IF = 0;      // step 5: clear interrupt flag
  IEC0bits.T2IE = 1;      // step 6: enable interrupt
  __builtin_enable_interrupts();      // step 7: enable interrupts at CPU

  while(1) { ; }
  return 0;
}
