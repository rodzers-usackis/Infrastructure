#define __DELAY_BACKWARD_COMPATIBLE__  
#include <avr/io.h>
#include <flashing_leds.h>
#include <util/delay.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <usart.h>
#include <ctype.h>

#define NUMBER_OF_LEDS 4
#define ALL_LEDS 0b00111100
#define TIME_UNIT 250

// NOTE: I'm using 250ms as 1 time unit. It might be hard to read, but those are the standards.

/*
There are rules to help people distinguish dots from dashes in Morse code.

The length of a dot is 1 time unit. 

A dash is 3 time units. 

The space between symbols (dots and dashes) of the same letter is 1 time unit.

The space between letters is 3 time units.

The space between words is 7 time units.
*/

void startApplication() {
  printf("Starting the application.\n");

  for (int i = 0; i<NUMBER_OF_LEDS; i++) {
    _delay_ms(1000);
    lightDownLed(i);
  }

  _delay_ms(2000);
}

void dot() {
  lightUpLeds(ALL_LEDS);
  _delay_ms(TIME_UNIT);
  lightDownLeds(ALL_LEDS);
  _delay_ms(TIME_UNIT);
}

void dash() {
  lightUpLeds(ALL_LEDS);
  _delay_ms(TIME_UNIT*3);
  lightDownLeds(ALL_LEDS);
  _delay_ms(TIME_UNIT);
}

void letterToMorse(char letter) {
  
  switch (letter) {
    case 'a':
      dot();
      dash();
      break;

    case 'b':
      dash();
      dot();
      dot();
      dot();
      break;

    case 'c':
      dash();
      dot();
      dash();
      dot();
      break;

    case 'd':
      dash();
      dot();
      dot();
      break;

    case 'e':
      dot();
      break;

    case 'f':
      dot();
      dot();
      dash();
      dot();
      break;

    case 'g':
      dash();
      dash();
      dot();
      break;

    case 'h':
      dot();
      dot();
      dot();
      dot();
      break;

    case 'i':
      dot();
      dot();
      break;

    case 'j':
      dot();
      dash();
      dash();
      dash();
      break;

    case 'k':
      dash();
      dot();
      dash();
      break;

    case 'l':
      dot();
      dash();
      dot();
      dot();
      break;

    case 'm':
      dash();
      dash();
      break;

    case 'n':
      dash();
      dot();
      break;

    case 'o':
      dash();
      dash();
      dash();
      break;

    case 'p':
      dot();
      dash();
      dash();
      dot();
      break;

    case 'q':
      dash();
      dash();
      dot();
      dash();
      break;

    case 'r':
      dot();
      dash();
      dot();
      break;

    case 's':
      dot();
      dot();
      dot();
      break;

    case 't':
      dash();
      break;

    case 'u':
      dot();
      dot();
      dash();
      break;

    case 'v':
      dot();
      dot();
      dot();
      dash();
      break;

    case 'w':
      dot();
      dash();
      dash();
      break;

    case 'x':
      dash();
      dot();
      dot();
      dash();
      break;

    case 'y':
      dash();
      dot();
      dash();
      dash();
      break;

    case 'z':
      dash();
      dash();
      dot();
      dot();
      break;

    case '0':
      dash();
      dash();
      dash();
      dash();
      dash();
      break;

    case '1':
      dot();
      dash();
      dash();
      dash();
      dash();
      break;

    case '2':
      dot();
      dot();
      dash();
      dash();
      dash();
      break;

    case '3':
      dot();
      dot();
      dot();
      dash();
      dash();
      break;

    case '4':
      dot();
      dot();
      dot();
      dot();
      dash();
      break;

    case '5':
      dot();
      dot();
      dot();
      dot();
      dot();
      break;

    case '6':
      dash();
      dot();
      dot();
      dot();
      dot();
      break;

    case '7':
      dash();
      dash();
      dot();
      dot();
      dot();
      break;

    case '8':
      dash();
      dash();
      dash();
      dot();
      dot();
      break;

    case '9':
      dash();
      dash();
      dash();
      dash();
      dot();
      break;

    case ' ':
      _delay_ms(TIME_UNIT*7); // 7 time units between words. Space indicates a new word is starting.
      break;
  }
}

void processWord(char *word) {
  for (int i = 0; i<strlen(word); i++) {
    letterToMorse(tolower(word[i])); // Upper case letters transformed to lower case to match switch statements.
                                    // IF IT'S REALLY NECESSARY I CAN LIMIT THE FOR LOOP TO MATCH THE 10 LETTER REQUIREMENT.
    printf("\nThe displayed letter was: %c\n", tolower(word[i]));
    _delay_ms(TIME_UNIT*3); // Space between letters.
  }

  _delay_ms(2000);
  printf("\nThe content of the morse code was: %s", word);
}

int main() {
  initUSART();
  enableLeds(ALL_LEDS);
  // lightUpLeds(ALL_LEDS);

  startApplication(); // As soon as the application starts, a countdown pattern is shown with the LEDs, so that the user can prepare himself.
                     // The countdown pattern is as follows: you first light the 4 LEDs, then 3, 2, 1 and 0 and then the real Morse code training starts.

  char wordString[] = "Hello World";

  processWord(wordString);

  knightriderLedTheme();

  return 0;
}