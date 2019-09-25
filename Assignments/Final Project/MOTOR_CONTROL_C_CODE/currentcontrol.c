#include "NU32.h"
#include <stdio.h>
#include "encoder.h"
#include "utilities.h"
#include "isense.h"
#include "currentcontrol.h"
#include "positioncontrol.h"

// ***GLOBAL VARIABLES DEFINED IN currentcontrol.h
extern volatile int DC;
extern volatile int KP;
extern volatile int KI;
extern volatile int MAarray[NUMSAMPS];      // measured values to plot
extern volatile int REFarray[NUMSAMPS];      // reference values to plot
extern volatile int StoringData;      // if this flag = 1, currently
extern volatile int counter;

// ***GLOBAL VARIABLES DEFINED IN currentcontrol.h
extern volatile int Iref;

// ***LOCAL VARIABLES:
static volatile int Waveform[NUMSAMPS];     // waveform
static volatile int I_actual;
static volatile int e_current = 0, e_INT = 0;

void PWM_init(void) {
  T2CONbits.TCKPS = 2;     // Timer2 prescaler N=4 (1:4)
  PR2 = 3999;              // period = (PR2+1) * N * 12.5 ns = 200 us, 5 kHz
  TMR2 = 0;                // initial TMR2 count is 0
  T2CONbits.ON = 1;        // turn on Timer2
  IPC2bits.T2IP = 3;      // step 4: priority
  IPC2bits.T2IS = 0;
  IFS0bits.T2IF = 0;      // step 5: clear interrupt flag
  IEC0bits.T2IE = 1;      // step 6: enable interrupt

  T3CONbits.TCKPS = 0;     // Timer3 prescaler N=1 (1:1)
  PR3 = 3999;              // period = (PR3+1) * N * 12.5 ns = 50 us, 20 kHz
  TMR3 = 0;                // initial TMR3 count is 0
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC1RS = 1000;             // duty cycle = OC1RS/(PR3+1) = 25%
  OC1R = 1000;              // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;        // turn on Timer3
  OC1CONbits.ON = 1;       // turn on OC1

  TRISDbits.TRISD0 = 0;
  TRISDbits.TRISD11 = 0;
}

void __ISR (_TIMER_2_VECTOR, IPL3SOFT) currentController (void){     // _TIMER_2_VECTOR = 8
  int Ic;
  switch (MODE) {
    case IDLE:
    {
      DC = 0;
      OC1RS = 0;
      LATDbits.LATD11 = 0;
      break;
    }

    case PWM:
    {
      if (DC < 0){
        LATDbits.LATD11 = 1;
      }

      else {
        LATDbits.LATD11 = 0;
      }
      OC1RS = 40 * abs(DC);
      break;
    }

    case ITEST:
    {
      I_actual = get_current();
      e_current = Waveform[counter] - I_actual;
      e_INT = e_INT + e_current;

      if (e_INT > EINTMAX) {
        e_INT = EINTMAX;
      }
      else if (e_INT < -EINTMAX){
        e_INT = EINTMAX;
      }

      Ic = KP*e_current + KI*e_INT;

      DC = Ic;
      if (DC <= 0){
        LATDbits.LATD11 = 1;
        DC = -DC;
      }

      else {
        LATDbits.LATD11 = 0;
      }
      OC1RS = DC;

      if (StoringData) {
        MAarray[counter] = I_actual;     // store data in global arrays
        REFarray[counter] = Waveform[counter];
        counter++;                      // increment plot data index

        if (counter == NUMSAMPS) {     // if max number of plot points is reached
          counter = 0;                // reset the plot index
          StoringData = 0;            // tell main data is ready to be sent to MATLAB
        }
      }
      break;
    }
    case HOLD:
    {
      I_actual = get_current();
      if (Iref==0) {
        I_actual = 0;
      }
      e_current = Iref - I_actual;
      e_INT = e_INT + e_current;

      if (e_INT > EINTMAX) {
        e_INT = EINTMAX;
      }
      else if (e_INT < -EINTMAX){
        e_INT = EINTMAX;
      }

      Ic = KP*e_current + KI*e_INT;

      DC = Ic;
      if (DC < 0){
        LATDbits.LATD11 = 1;
        DC = -DC;
      }

      else {
        LATDbits.LATD11 = 0;
      }
      OC1RS = DC;
      break;
    }

    case TRACK:
    {
      I_actual = get_current();
      if (Iref==0) {
        I_actual = 0;
      }
      e_current = Iref - I_actual;
      e_INT = e_INT + e_current;

      if (e_INT > EINTMAX) {
        e_INT = EINTMAX;
      }
      else if (e_INT < -EINTMAX){
        e_INT = EINTMAX;
      }

      Ic = KP*e_current + KI*e_INT;

      DC = Ic;
      if (DC < 0){
        LATDbits.LATD11 = 1;
        DC = -DC;
      }

      else {
        LATDbits.LATD11 = 0;
      }
      OC1RS = DC;
      break;
    }
  }
  IFS0bits.T2IF = 0;      // clearing the interrupt flag
}

void makeWaveform() {
  int i = 0, center = 0, A = 200;     // square wave, fill in center value and amplitude
  for (i = 0; i < NUMSAMPS; i++) {
    if (i<25) {
      Waveform[i] = center + A;
    } else if (i>=25 && i<50) {
      Waveform[i] = center - A;
    } else if (i>=50 && i<75) {
      Waveform[i] = center + A;
    } else {
      Waveform[i] = center - A;
    }
  }
}
