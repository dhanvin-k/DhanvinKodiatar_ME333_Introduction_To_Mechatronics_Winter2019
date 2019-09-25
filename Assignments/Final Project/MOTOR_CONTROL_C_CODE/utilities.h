#ifndef utilities__H__
#define utilities__H__

// Enum used for mode:

typedef enum {
  IDLE = 1, PWM, ITEST, HOLD, TRACK
} modes;

// Global variables:

volatile modes MODE;

// utilities.c function prototypes:

void get_mode(modes);

#endif // utilities__H__
