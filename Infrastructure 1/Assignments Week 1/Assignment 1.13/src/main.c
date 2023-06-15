#include <util/delay.h>
#include <avr/io.h>
#include <flashing_leds.h>
#include <stdlib.h>
#include <time.h>

#define __DELAY_BACKWARD_COMPATIBLE__  

int generateRandomLed() {
   int min = 0;
   int max = 3;

   return (rand() % (max - min + 1)) + min;
}

int generateRandomDelay() {
   int minDelay = 100;
   int maxDelay = 1000;

   return (rand() % (maxDelay - minDelay + 1)) + minDelay;
}

// Write a program that makes the various LEDs flash in a random pattern:
// randomly selected LEDs are switched on and off for arbitrarily long periods (between 100 and 1000 milliseconds, for instance).
int main() { 
  uint8_t allLeds = 0b00111100;
  srand(0);

  enableLeds(allLeds);
  lightDownLeds(allLeds);

   while (1) {
      int selectedLed = generateRandomLed();

      lightUpLed(selectedLed);
      _delay_ms(generateRandomDelay());
      lightDownLed(selectedLed);
      _delay_ms(generateRandomDelay());

   }
   
   return 0;
}

