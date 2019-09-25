#include "NU32.h"
#include <stdio.h>
#include "encoder.h"
#include "utilities.h"
#include "isense.h"
#include "currentcontrol.h"
#include "positioncontrol.h"

extern volatile int desired_angle;
extern volatile int Iref;

static volatile int e = 0, eint = 0, edot = 0, eprev = 0;

extern volatile int N;
extern volatile int refTraj[2000];
extern volatile int refINDEX;
extern volatile int MOTOarray[2000];
extern volatile int TIMEarray[2000];
extern volatile int holding_angle;

void position_init(void) {
  T4CONbits.TCKPS = 4;     // Timer4 prescaler N=64 (1:64)
  PR4 = 6249;              // period = (PR4+1) * N * 12.5 ns = 400 us, 200 Hz
  TMR4 = 0;                // initial TMR4 count is 0
  T4CONbits.ON = 1;        // turn on Timer2
  IPC4bits.T4IP = 4;      // step 4: priority
  IPC4bits.T4IS = 0;
  IFS0bits.T4IF = 0;      // step 5: clear interrupt flag
  IEC0bits.T4IE = 1;      // step 6: enable interrupt
}

void __ISR (_TIMER_4_VECTOR, IPL4SOFT) positionController (void){     // _TIMER_2_VECTOR = 8
  switch (MODE) {
    case HOLD:
    {
      int actual_angle = get_angle();
      e = desired_angle - actual_angle;
      eint = eint + e;
      edot = edot - eprev;
      Iref = KP_pos*e + KI_pos*eint + KD_pos*edot;
      eprev = e;
      break;
    }
    case TRACK:
    {
      int actual_angle = get_current();
      desired_angle = refTraj[refINDEX];
      e = desired_angle - actual_angle;
      eint = eint + e;
      edot = edot - eprev;
      Iref = KP_pos*e + KI_pos*eint + KD_pos*edot;
      eprev = e;

      if (StoringData) {
        MOTOarray[refINDEX] = actual_angle;     // store data in global arrays
        TIMEarray[refINDEX] = refTraj[refINDEX];
        refINDEX++;

        if (refINDEX == N) {     // if max number of plot points is reached
          desired_angle = refTraj[refINDEX];                // reset the plot index
          StoringData = 0;            // tell main data is ready to be sent to MATLAB
          MODE = HOLD;
        }
      }
      break;
    }
  }
  IFS0bits.T4IF = 0;      // clearing the interrupt flag
}
