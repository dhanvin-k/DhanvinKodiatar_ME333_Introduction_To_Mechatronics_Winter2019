#ifndef currentcontrol__H__
#define currentcontrol__H__

#define NUMSAMPS 100      // number of points in waveform
#define EINTMAX 20

volatile int DC;
volatile float KP;    // current gains
volatile float KI;

volatile int MAarray[NUMSAMPS];      // measured values to plot
volatile int REFarray[NUMSAMPS];      // reference values to plot
volatile int StoringData;      // if this flag = 1, currently
volatile int counter;

// currentcontrol.c function prototypes:

void PWM_init(void);
void makeWaveform();

#endif // currentcontrol__H__
