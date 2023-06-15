#define __DELAY_BACKWARD_COMPATIBLE__  

#include <avr/io.h>
#include <flashing_leds.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <usart.h>

// NOTE: If something doesn't work it's because I changed flashingLeds();
int main() {
  initUSART();
  uint8_t allLeds = 0b00111100; 
  srand(0);

// enableLeds(allLeds);

  // 1. Write a program to light up the 4 LEDs of the shield each in their own turn
  // NOTE: Didn't quite understand the task.

  for (int i = 0; i<4; i++) {
    enableLed(i);
    lightUpLed(i); // Redundant
    _delay_ms(350);
  }


  lightDownLeds(allLeds);
  _delay_ms(1000);

  // 2. Write a program to make one of the LEDs light exactly 10 times

  for (int i = 0; i<10; i++) {
    ledOnAndOffWithDelay(3, 50);
  }

  // 3. Turn exercise 2 into a function, where the parameters are the LED number and the number of flashes.
  _delay_ms(1000);
  flashingLed(3, 10, 50);

  // 4. Write a program that makes an LED burn longer and longer (start with 10 ms and rise up to 1000 ms, incrementing with 50 ms in every iteration.
  _delay_ms(1000);
  
  for(int i = 10; i<=1000; i+=50)  {
    lightUpLed(3);
    _delay_ms(i);
    lightDownLed(3);
    _delay_ms(100);
  }

  // 5. Write a program that uses an array of 10 digits between 0 and 4. Walk through the array and light up the corresponding led.
  int ledArray[10];
  int lower = 0;
  int upper = 3;

  for (int i = 0; i<10; i++) { 
    int num = (rand() % (upper - lower + 1)) + lower;
    ledArray[i] = num;
    
    ledOnAndOffWithDelay(ledArray[i], 100);

  }

  // 6. Write a program that creates an empty array and then fills it with the numbers from 10 to 1000 with a step value of 50 ms.
  // Use this array to light up a led, just like in exercise 4.
  int count = 0;

  for (int i = 10; i<=1000; i+=50) { // Finding out the size I need :)
                                    // Might be redundant, but I don't know what's worse, setting the size based on a guess or doing this.
                                   // (Assuming I'm working with larger numbers and can't just calculate it easily.)
    count++;
  }

  int emptyArray[count];
  int indexNumber = 0;

for (int i = 10; i<=1000; i+=50) { // Creating the array and using it simultaneously, just like in the previous exercise.
    emptyArray[indexNumber] = i;
    lightUpLed(3);
    _delay_ms(emptyArray[indexNumber]);
    lightDownLed(3);
    _delay_ms(100);
    indexNumber++;
  }

  // 7. Write a program that makes the leds light up based on a string. 
  // The string contains a sequence of the letters a, b, c and d. An 'a' lights the first LED, 'b' the second and so on.

  char ledString[] = "abcdabcd";

  for (int i = 0; i<strlen(ledString); i++) {

    switch(ledString[i]) {
      case 'a':
        ledOnAndOffWithDelay(0, 100);
        break;

      case 'b':
        ledOnAndOffWithDelay(1, 100);
        break;

      case 'c':
        ledOnAndOffWithDelay(2, 100);
        break;

      case 'd':
        ledOnAndOffWithDelay(3, 100);
        break;

      default:
        lightUpLeds(allLeds);
    }
  }

  // 8. Write a program that lights up the leds based on a string. But this time the string of letters is random. 
  // With the letter 'a' all 4 LEDs light up once, with a letter 'b' they light up twice, and so on.

  char alphabetPool[] = "abcdefghijklmnopqrstuvwxyz";
  int poolSize = strlen(alphabetPool);

  int minLength = 1; 
  int maxLength = 6; 
  int stringSize = (rand() % (maxLength - minLength + 1)) + minLength; // Randomizer that generates the length of the string.

  char newString[stringSize];

  for (int i= 0; i<stringSize; i++) { 
    newString[i] = alphabetPool[rand() % poolSize];
  }

  printf("\nThe new, randomized string is: %s\n", newString);

  for (int i = 0; i<stringSize; i++) {
    const char *ptr = strchr(alphabetPool, newString[i]);

    int index = ptr - alphabetPool;
    printf("\nFlashing led %d times for the letter %c.\n", index+1, newString[i]);
    flashingLeds(allLeds, index+1, 50);
    _delay_ms(3000);
     
  }
  
  lightDownLeds(allLeds);

  return 0;

}
