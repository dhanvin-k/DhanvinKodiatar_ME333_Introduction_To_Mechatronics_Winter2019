#ifndef positioncontrol__H__
#define positioncontrol__H__

volatile int desired_angle;
volatile float KP_pos;   // position gains
volatile float KI_pos;
volatile float KD_pos;
volatile int Iref;

volatile int N;
volatile int refINDEX;
volatile int refTraj[2000];
volatile int MOTOarray[2000];
volatile int TIMEarray[2000];
volatile int holding_angle;

// positioncontrol.c function prototypes:

void position_init(void);

#endif // positioncontrol__H__
