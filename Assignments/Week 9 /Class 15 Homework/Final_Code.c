#include "NU32.h"          // constants, functions for startup and UART
#include <stdio.h>

#define  NUMSAMPS 1000      // number of points in waveform
#define PLOTPTS 200         // number of data points to plot
#define DECIMATION 10       // plot every 10th point
#define SAMPLE_TIME 10       // 10 core timer ticks = 250 ns
static volatile int Waveform[NUMSAMPS];     // waveform
static volatile int ADCarray[PLOTPTS];      // measured values to plot
static volatile int REFarray[PLOTPTS];      // reference values to plot
static volatile int StoringData = 0;      // if this flag = 1, currently storing plot data
static volatile float Kp = 0, Ki = 0;     // Control gains
static volatile int u = 0, unew = 0, e = 0, Eint = 0;

unsigned int adc_sample_convert(int pin) { // sample & convert the value on the given
                                           // adc pin the pin should be configured as an
                                           // analog input in AD1PCFG
    unsigned int elapsed = 0, finish_time = 0;
    AD1CHSbits.CH0SA = pin;                // connect chosen pin to MUXA for sampling
    AD1CON1bits.SAMP = 1;                  // start sampling
    elapsed = _CP0_GET_COUNT();
    finish_time = elapsed + SAMPLE_TIME;
    while (_CP0_GET_COUNT() < finish_time) {
      ;                                   // sample for more than 250 ns
    }
    AD1CON1bits.SAMP = 0;                 // stop sampling and start converting
    while (!AD1CON1bits.DONE) {
      ;                                   // wait for the conversion process to finish
    }
    return ADC1BUF0;                      // read the buffer with the result
}

void makeWaveform() {
  int i = 0, center = 500, A = 300;     // square wave, fill in center value and amplitude
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
  static int plotind = 0;     // index for data arrays; counts up to PLOTPTS
  static int decctr = 0;      // counts to store data one every DECIMATION
  static int adcval = 0;      // 

  OC1RS = (unsigned int) ((unew/100.0) * PR3);

  adcval = adc_sample_convert(0);

  e = Waveform[counter] - adcval;
  Eint = Eint + e;
  u = Kp*e + Ki*Eint;
  unew = u + 50.0;
  if (unew > 100.0) {
    unew = 100.0;
  } else if (unew < 0.0) {
    unew = 0.0;
  }

  if (StoringData) {
    decctr++;
    if (decctr == DECIMATION) {       // after DECIMATION control loops
      decctr = 0;                     // reset decimation counter
      ADCarray[plotind] = adcval;     // store data in global arrays
      REFarray[plotind] = Waveform[counter];
      plotind++;                      // increment plot data index
    }
    if (plotind == PLOTPTS) {     // if max number of plot points is reached
      plotind = 0;                // reset the plot index
      StoringData = 0;            // tell main data is ready to be sent to MATLAB
    }
  }
  counter++;        // add one to counter every time ISR is entered
  if (counter == NUMSAMPS) {
    counter = 0;      // Roll the counter over when needed
  }
  IFS0bits.T2IF = 0;      // clearing the interrupt flag
}

int main(void) {
  NU32_Startup();          // cache on, interrupts on, LED/button init, UART init
  char message[100];       // message to and from MATLAB
  float kptemp = 0, kitemp = 0;     // temporary local gains
  int i = 0;               // plot data counter
  makeWaveform();
  T3CONbits.TCKPS = 0;     // Timer3 prescaler N=1 (1:1)
  PR3 = 3999;              // period = (PR3+1) * N * 12.5 ns = 50 us, 20 kHz
  TMR3 = 0;                // initial TMR2 count is 0
  OC1CONbits.OCM = 0b110;  // PWM mode without fault pin; other OC1CON bits are defaults
  OC1CONbits.OCTSEL = 1;
  OC1RS = 3000;             // duty cycle = OC1RS/(PR3+1) = 75%
  OC1R = 3000;              // initialize before turning OC1 on; afterward it is read-only
  T3CONbits.ON = 1;        // turn on Timer3
  OC1CONbits.ON = 1;       // turn on OC1

  AD1PCFGbits.PCFG0 = 0;                 // AN0 is an adc pin
  AD1CON3bits.ADCS = 2;                   // ADC clock period is Tad = 2*(ADCS+1)*Tpb =
                                          //                           2*3*12.5ns = 75ns
  AD1CON1bits.ADON = 1;                   // turn on A/D converter

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

  while(1) {
  NU32_ReadUART3(message, sizeof(message));     // wait for message from MATLAB
  sscanf(message, "%f %f", &kptemp, &kitemp);
  __builtin_disable_interrupts();     // keep ISR disabled as brief as possible
  Kp = kptemp;      // copy local variables to global used by ISR
  Ki = kitemp;
  __builtin_enable_interrupts();      // only 2 simple C commands while ISRs disabled
  Eint = 0;                           // Integrator anti-windup
  StoringData = 1;                    // message to ISR to start storing data
  while (StoringData) { ; }           // wait until ISR says data storing is done
  for (i = 0; i < PLOTPTS; i++) {     // send plot data to MATLAB
                                      // when first number sent = 1, MATLAB knows we're done
    sprintf(message, "%d %d %d\r\n", PLOTPTS - i, ADCarray[i], REFarray[i]);
    NU32_WriteUART3(message);
  }
  }
  return 0;
}
