/*  bubble sort:

    This is program takes an user inputted string of characters and sorts the
    characters in assending order according to their ASCII value, using the
    bubble sort method. */

#include <stdio.h>
#include <string.h>
#define MAXLENGTH 100   // max length of string input

// helper prototypes:
void getString(char *str);
void printResult(char *str);
int greaterThan(char ch1, char ch2);
void swap(char *str, int index1, int index2);

// main function:
int main(void) {
  int len, count;
  char str[MAXLENGTH];    // input should be no longer than MAXLENGTH

  getString(str);   // getting the string to be sorted
  len = strlen(str);    // get length of the string, from string.h
  count = len-1;    // counter for our second for loop initialized

  // nested for loops:
  for (int i = 0; i < len-1; i++) {
    for (int j = 0; j < count; j++) {
      if (greaterThan(str[j], str[j+1]) == 1) {
      swap(str, j, j+1);  // swaping elements at j and j+1 if j>j+1
      }
    }
    count--;    // counter decreasing because
                // elements are bubbled in each loop
  }

  printResult(str);   // printing the sorted string
  return 0;
}

// Helper functions:

void getString(char *str) {
  printf("Enter string (max characters 100): ");
  scanf("%s", str);
}

void printResult(char *str) {
  printf("Result: %s\n", str);
}

int greaterThan(char ch1, char ch2) {
  if (ch1>ch2) {
    return 1;
  }

  else {
    return 0;
  }
}

void swap(char *str, int index1, int index2) {
  str[index1] = str[index1] + str[index2];
  str[index2] = str[index1] - str[index2];
  str[index1] = str[index1] - str[index2];
}
