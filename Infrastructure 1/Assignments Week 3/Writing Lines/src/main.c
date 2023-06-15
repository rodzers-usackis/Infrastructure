#include <buttons.h>
#include <usart.h>
#include <util/delay.h>
#include <avr/io.h>
#include <string.h>
#include <stdlib.h>

int reservedSpace = 0;

// Assignment 2.
void writeLinesOnHeap(char sentence []) {
  int sentenceLength = strlen(sentence) + 1; // +1 because strlen() does not include the \0 character while returning length.
  char* heap;
  heap = malloc(sentenceLength * sizeof(char));

  if (heap == NULL) {
    printf("\n\nMemory allocation failed. Ending program.");
    exit(1);
  }

  strcpy(heap, sentence);
  reservedSpace += sentenceLength;
  printf("\n\"%s\" is on heap ...", sentence);
  // Extra.
  printf("\n%d bytes are now occupied on heap ...", reservedSpace);
}

int main() {
  initUSART();
  enableButton(0);
  char string[] = "I am not allowed to speak in class.";
  
  // Assignment 1.
  while (1) {
    if (buttonPushed(0)==1) {
      for(int i = 1; i <= 100; i++) {
        printf("\n%d: %s", i, string);
        writeLinesOnHeap(string); // I think there's an overflow, return later ? Since we don't free() the allocated memory ?
      }
    }
  }

  return 0;
}