#ifndef utilities__H__
#define utilities__H__

typedef enum {
  IDLE = 1, PWM, ITEST, HOLD, TRACK
} modes;

volatile modes MODE;

// utilities.c function prototypes:

void get_mode(modes);

#endif // utilities__H__
