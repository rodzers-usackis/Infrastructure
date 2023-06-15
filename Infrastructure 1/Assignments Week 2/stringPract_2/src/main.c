#include <avr/io.h>
#include <usart.h>
#include <string.h>

#define MAX 7

void printFirstLetters(char names[MAX][10]) {
  printf("\nFirst letters: ");

  for (int i = 0; i < MAX; i++) {
    printf("%c", names[i][0]);
  }
}

void printLastLetters(char names[MAX][10]) {
  printf("\nLast letters: ");

  for (int i = 0; i < MAX; i++) {
    printf("%c", names[i][strlen(names[i])-1]);
  }
}

char* searchShortestName(char names[MAX][10]) {
  int shortestIndex = 0;
  
  for (int i = 0; i < MAX; i++) {
    if ((int)strlen(names[i])<(int)strlen(names[shortestIndex])) {
      shortestIndex = i;
    }
  }
  return names[shortestIndex];
}



int main() {
  initUSART();

  char names[MAX][10] = {"Moe", "Larry", "Curly", "Adam", "Peter", "John", "Jon"};

  printFirstLetters(names);

  printLastLetters(names);

  char* smallest = searchShortestName(names);

  printf("\nShortest = %s", smallest);

  return 0;
}