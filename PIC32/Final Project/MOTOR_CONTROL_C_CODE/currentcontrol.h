#ifndef currentcontrol__H__
#define currentcontrol__H__

// Global Constants:

#define NUMSAMPS 100      // number of points in waveform
#define EINTMAX 20

// Global variables:

volatile int DC;
volatile int KP;    // current gains
volatile int KI;
volatile int MAarray[NUMSAMPS];      // measured values to plot
volatile int REFarray[NUMSAMPS];      // reference values to plot
volatile int StoringData;      // if this flag = 1, currently
volatile int counter;

// currentcontrol.c function prototypes:

void PWM_init(void);
void makeWaveform();

#endif // currentcontrol__H__
