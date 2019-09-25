#include <stdio.h>
#include "NU32.h"

#define MAX_YEARS 100


typedef struct {
  double inv0;
  double growth;
  int years;
  double invarray[MAX_YEARS+1];
} Investment;


int getUserInput(Investment *invp);
void calculateGrowth(Investment *invp);
void sendOutput(double *arr, int years);


int main(void) {

  Investment inv;

  while(getUserInput(&inv)) {
    inv.invarray[0] = inv.inv0;
    calculateGrowth(&inv);
    sendOutput(inv.invarray,
               inv.years);
  }
  return 0;
}


void calculateGrowth(Investment *invp) {

  int i;

  // for loop ==SecA.4.13==
  for (i = 1; i <= invp->years; i= i + 1) {

    invp->invarray[i] = invp->growth * invp->invarray[i-1];
  }
}


int getUserInput(Investment *invp) {

  int valid;
  char statement[100], value[100], validStatement[100], invalidStatement[100];

  //  Altered printf statment here:
  sprintf(statement,"Enter investment, growth rate, number of yrs (up to %d): ",MAX_YEARS);
  NU32_WriteUART3(statement);

  //  Altered scanf statment here:
  NU32_ReadUART3(value, 100);
  sscanf(value,"%lf %lf %d", &(invp->inv0), &(invp->growth), &(invp->years));

  valid = (invp->inv0 > 0) && (invp->growth > 0) &&
    (invp->years > 0) && (invp->years <= MAX_YEARS);

  //  Altered printf statement here:
  sprintf(validStatement,"Valid input?  %d\n",valid);
  NU32_WriteUART3(validStatement);

  if (!valid) {

    //  Altered printf statement here:
    sprintf(invalidStatement,"Invalid input; exiting.\n");
    NU32_WriteUART3(invalidStatement);
  }
  return(valid);
}


void sendOutput(double *arr, int yrs) {

  int i;
  char outstring[100], resultStatement[100], *newLine;

  //Altered printf statement here:
  sprintf(resultStatement,"\nRESULTS:\n\n");
  NU32_WriteUART3(resultStatement);
  for (i=0; i<=yrs; i++) {
    sprintf(outstring,"Year %3d:  %10.2f\n",i,arr[i]);

    //  Altered printf statement here:
    NU32_WriteUART3(outstring);
  }
  //  Altered printf statement for new line here:
  sprintf(newLine,"\n");
  NU32_WriteUART3(newLine);
}
