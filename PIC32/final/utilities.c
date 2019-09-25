#include "currentcontrol.h"
#include "positioncontrol.h"
#include "utilities.h"
#include "isense.h"
#include "encoder.h"
#include "NU32.h"
#include <stdio.h>

void get_mode(modes m) {
  char status[50];
  switch (m) {
    case IDLE:
    {
      sprintf(status,"Current mode: IDLE\r\n");
      NU32_WriteUART3(status);
      break;
    }
    case PWM:
    {
      sprintf(status,"Current mode: PWM\r\n");
      NU32_WriteUART3(status);
      break;
    }
    case ITEST:
    {
      sprintf(status,"Current mode: ITEST\r\n");
      NU32_WriteUART3(status);
      break;
    }
    case HOLD:
    {
      sprintf(status,"Current mode: HOLD\r\n");
      NU32_WriteUART3(status);
      break;
    }
    case TRACK:
    {
      sprintf(status,"Current mode: TRACK\r\n");
      NU32_WriteUART3(status);
      break;
    }
  }
}
