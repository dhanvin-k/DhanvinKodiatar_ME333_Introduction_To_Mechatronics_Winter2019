#include "NU32.h"
#include <stdio.h>
#include "encoder.h"
#include "utilities.h"
#include "isense.h"
#include "currentcontrol.h"
#include "positioncontrol.h"

#define BUF_SIZE 200

int main()
{
  char buffer[BUF_SIZE];

  // ***GLOBAL VARIABLES DEFINED utilities.h***
  extern volatile modes MODE;

  // ***GLOBAL VARIABLES DEFINED IN currentcontrol.h***
  extern volatile int DC;
  extern volatile int KP;   // current gains
  extern volatile int KI;
  extern volatile int MAarray[NUMSAMPS];      // measured values to plot
  extern volatile int REFarray[NUMSAMPS];      // reference values to plot

  // ***GLOBAL VARIABLES DEFINED IN positioncontrol.h***
  extern volatile int desired_angle;
  extern volatile int KP_pos;   // position gains
  extern volatile int KI_pos;
  extern volatile int KD_pos;
  extern volatile int N;
  extern volatile int refINDEX;
  extern volatile int refTraj[2000];
  extern volatile int MOTOarray[2000];
  extern volatile int TIMEarray[2000];
  extern volatile int holding_angle;

  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;


  __builtin_disable_interrupts();
  encoder_init();
  ADC_init();
  PWM_init();
  position_init();

  MODE = IDLE;
  __builtin_enable_interrupts();


  while(1)
  {
    NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32_LED2 = 1;                   // clear the error LED

    switch (buffer[0]) {

      case 'a':
      {
        sprintf(buffer, "%d\r\n", ADC_avg());   // ADC_avg() is in isense.c
        NU32_WriteUART3(buffer);
        break;
      }

      case 'b':
      {
        sprintf(buffer, "%d\r\n", get_current());   // get_current() is in isense.c
        NU32_WriteUART3(buffer);
        break;
      }

      case 'c':
      {
        sprintf(buffer, "%d\r\n", encoder_counts());  // encoder_counts is in encoder.c
        NU32_WriteUART3(buffer);    // send encoder count to client in 'counts'
        break;
      }

      case 'd':
      {
        sprintf(buffer, "%d\r\n", get_angle());   // get_angle() is in encoder.c
        NU32_WriteUART3(buffer);    // send encoder count to client in 'deg'
        break;
      }

      case 'e':
      {
        sprintf(buffer, "%d\r\n", encoder_reset()); // encoder_reset() is in encoder.c
        NU32_WriteUART3(buffer);    // send encoder count to client
        break;
      }

      case 'f':
      {
        MODE = PWM;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &DC);
        break;
      }

      case 'g':
      {
        __builtin_disable_interrupts();
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &KP);

        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &KI);
        __builtin_enable_interrupts();
        break;
      }

      case 'h':
      {
        sprintf(buffer, "%d\r\n", KP);
        NU32_WriteUART3(buffer);

        sprintf(buffer, "%d\r\n", KI);
        NU32_WriteUART3(buffer);
        break;
      }

      case 'i':
      {
        __builtin_disable_interrupts();
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &KP_pos);

        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &KI_pos);

        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &KD_pos);
        __builtin_enable_interrupts();
        break;
      }

      case 'j':
      {
        sprintf(buffer, "%d\r\n", KP_pos);
        NU32_WriteUART3(buffer);

        sprintf(buffer, "%d\r\n", KI_pos);
        NU32_WriteUART3(buffer);

        sprintf(buffer, "%d\r\n", KD_pos);
        NU32_WriteUART3(buffer);
        break;
      }

      case 'k':
      {
        __builtin_disable_interrupts();
        makeWaveform();
        MODE = ITEST;
        counter = 0;
        StoringData = 1;                    // message to ISR to start storing data
        __builtin_enable_interrupts();
        while (StoringData) { ; }           // wait until ISR says data storing is done

        MODE = IDLE;

        sprintf(buffer, "%d\r\n", NUMSAMPS);
        NU32_WriteUART3(buffer);
        int i = 0;
        for (i = 0; i < NUMSAMPS; i++) {     // send plot data to MATLAB
                                            // when first number sent = 1, MATLAB knows we're done
          sprintf(buffer, "%d %d\r\n", REFarray[i], MAarray[i]);
          NU32_WriteUART3(buffer);
        }
        break;
      }

      case 'l':
      {
        __builtin_disable_interrupts();
        encoder_reset();
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &desired_angle);
        __builtin_enable_interrupts();
        MODE = HOLD;
        break;
      }

      case 'm':
      {
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &N);
        int i = 0;
        for(i = 0; i<N; i++) {
          NU32_ReadUART3(buffer,BUF_SIZE);
          sscanf(buffer, "%d", &refTraj[i]);
        }
        break;
      }

      case 'n':
      {
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &N);
        int i = 0;
        for(i = 0; i<N; i++) {
          NU32_ReadUART3(buffer,BUF_SIZE);
          sscanf(buffer, "%d", &refTraj[i]);
        }
        break;
      }

      case 'o':
      {
        __builtin_disable_interrupts();
        refINDEX = 0;
        StoringData = 1;
        MODE = TRACK;
        __builtin_enable_interrupts();

        while (StoringData) { ; }           // wait until ISR says data storing is done
        sprintf(buffer, "%d\r\n", N);
        NU32_WriteUART3(buffer);
        int i = 0;
        for (i = 0; i < N; i++) {     // send plot data to MATLAB
                                            // when first number sent = 1, MATLAB knows we're done
          sprintf(buffer, "%d %d\r\n", TIMEarray[i], MOTOarray[i]);
          NU32_WriteUART3(buffer);
        }
        MODE = HOLD;
        break;
      }

      case 'p':
      {
        MODE = IDLE;
        break;
      }

      case 'q':
      {
        MODE = IDLE;
        break;
      }

      case 'r':
      {
        get_mode(MODE);
        break;
      }

      default:
      {
        NU32_LED2 = 0;  // turn on LED2 to indicate an error
        break;
      }
    }
  }
  return 0;
}
