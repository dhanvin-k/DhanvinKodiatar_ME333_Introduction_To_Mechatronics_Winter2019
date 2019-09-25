#ifndef helper__H__
#define helper__H__

//  This section defines global variables:
#define MAX_YEARS 100

//  This section defines a structure:
typedef struct {
  double inv0;
  double growth;
  int years;
  double invarray[MAX_YEARS+1];
} Investment;

//  This section defines the function prototypes:
int getUserInput(Investment *invp);
void calculateGrowth(Investment *invp);
void sendOutput(double *arr, int years);

#endif
