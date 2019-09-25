#include <stdio.h>
#include <math.h>
#include "NU32.h"

int main() {
  float f1 = 2.07, f2;
  long double d1 = 2.07, d2;
  unsigned int elapsedtick[4], elapsedns[4];

  _CP0_SET_COUNT(0);
  f2 = cosf(f1);
  elapsedtick[0] = _CP0_GET_COUNT();
  elapsedns[0] = elapsedtick[0] * 25;

  _CP0_SET_COUNT(0);
  f2 = sqrtf(f1);
  elapsedtick[1] = _CP0_GET_COUNT();
  elapsedns[1] = elapsedtick[1] * 25;

  _CP0_SET_COUNT(0);
  d2 = cos(d1);
  elapsedtick[2] = _CP0_GET_COUNT();
  elapsedns[2] = elapsedtick[2] * 25;

  _CP0_SET_COUNT(0);
  d2 = sqrt(d1);
  elapsedtick[3] = _CP0_GET_COUNT();
  elapsedns[3] = elapsedtick[3] * 25;

  return 0;
}
