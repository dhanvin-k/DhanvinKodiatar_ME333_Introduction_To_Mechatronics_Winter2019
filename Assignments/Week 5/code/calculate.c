#include "calculate.h"
#include "io.h"

//This section gives the calculate function definitions of the function prototypes
//defined in the helper.h file:

void calculateGrowth(Investment *invp) {

  int i;

  // for loop ==SecA.4.13==
  for (i = 1; i <= invp->years; i= i + 1) {

    invp->invarray[i] = invp->growth * invp->invarray[i-1];
  }
}
