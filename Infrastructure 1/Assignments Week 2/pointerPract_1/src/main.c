#include <avr/io.h>
#include <usart.h>

// Define 2 Macros.
#define MAX 5
#define SEVENFOLD(number) (number*7)

void printArray(int *array) {
  printf("\nContent of array: \n");
  
  for (int i = 0; i<MAX; i++) { // NOTE: i<sizeof(array) only prints out 2 pointers.
    printf("Address: %p has value: %d\n", &array[i], array[i]); // %p - pointers
  }
}

void makeArray(int *array) {
  for (int i = 0; i<MAX; i++) {
    array[i] = SEVENFOLD(i);
  }
}

int main() {
  initUSART();

  int array[MAX] = {0}; // Creating array of MAX integers, and initializing with 0.

  printArray(array);

  makeArray(array);

  printArray(array);

  return 0;
}