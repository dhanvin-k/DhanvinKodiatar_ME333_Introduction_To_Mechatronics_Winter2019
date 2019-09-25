#include "currentcontrol.h"
#include "positioncontrol.h"
#include "utilities.h"
#include "isense.h"
#include "encoder.h"
#include "NU32.h"
#include <stdio.h>

#define BUF_SIZE 200

int main()
{
  char buffer[BUF_SIZE];
  extern volatile modes MODE;
  extern volatile int DC;
  extern volatile float KP;   // current gains
  extern volatile float KI;
  extern volatile float KP_pos;   // position gains
  extern volatile float KI_pos;
  extern volatile float KD_pos;

  extern volatile int N;
  extern volatile int refTraj[2000];
  extern volatile int refINDEX;

  // position variables:
  extern volatile int desired_angle;

  NU32_Startup(); // cache on, min flash wait, interrupts on, LED/button init, UART init
  NU32_LED1 = 1;  // turn off the LEDs
  NU32_LED2 = 1;
  __builtin_disable_interrupts();
  encoder_init();
  MODE = IDLE;

  ADC_init();
  PWM_init();
  position_init();

  __builtin_enable_interrupts();

  while(1)
  {
    NU32_ReadUART3(buffer,BUF_SIZE); // we expect the next character to be a menu command
    NU32_LED2 = 1;                   // clear the error LED
    switch (buffer[0]) {
      case 'a':
      {
        sprintf(buffer, "%d\r\n", ADC_avg());
        NU32_WriteUART3(buffer);
        break;
      }
      case 'b':
      {
        int current = (1.841 * (ADC_avg()) - 1015.7);
        sprintf(buffer, "%d\r\n", current);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'c':
      {
        sprintf(buffer, "%d\r\n", encoder_counts());
        NU32_WriteUART3(buffer);    // send encoder count to client in 'counts'
        break;
      }
      case 'd':                      // dummy command for demonstration purposes
      {
        int enc_deg = get_angle();
        sprintf(buffer, "%d\r\n", enc_deg);
        NU32_WriteUART3(buffer);    // send encoder count to client in 'deg'
        break;
      }
      case 'e':
      {
        sprintf(buffer, "%d\r\n", encoder_reset());
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
        sscanf(buffer, "%f", &KP);

        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f", &KI);
        __builtin_enable_interrupts();
        break;
      }
      case 'h':
      {
        sprintf(buffer, "%f\r\n", KP);
        NU32_WriteUART3(buffer);

        sprintf(buffer, "%f\r\n", KI);
        NU32_WriteUART3(buffer);
        break;
      }
      case 'i':
      {
        __builtin_disable_interrupts();
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f", &KP_pos);

        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f", &KI_pos);

        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%f", &KD_pos);
        __builtin_enable_interrupts();
        break;
      }
      case 'j':
      {
        sprintf(buffer, "%f\r\n", KP_pos);
        NU32_WriteUART3(buffer);

        sprintf(buffer, "%f\r\n", KI_pos);
        NU32_WriteUART3(buffer);

        sprintf(buffer, "%f\r\n", KD_pos);
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
          sprintf(buffer, "%d %d\r\n", MAarray[i], REFarray[i]);
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
        int i = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &N);
        for(i = 0; i<N; i++) {
          NU32_ReadUART3(buffer,BUF_SIZE);
          sscanf(buffer, "%d", &refTraj[i]);
        }
        break;
      }
      case 'n':
      {
        int i = 0;
        NU32_ReadUART3(buffer,BUF_SIZE);
        sscanf(buffer, "%d", &N);
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

        desired_angle = holding_angle;
        MODE = HOLD;

        sprintf(buffer, "%d\r\n", N);
        NU32_WriteUART3(buffer);
        int i = 0;
        for (i = 0; i < N; i++) {     // send plot data to MATLAB
                                            // when first number sent = 1, MATLAB knows we're done
          sprintf(buffer, "%d %d\r\n", MOTOarray[i], TIMEarray[i]);
          NU32_WriteUART3(buffer);
        }
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
