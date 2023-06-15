#include <avr/io.h>
#include <flashing_leds.h>
#include <buttons.h>
#include <util/delay.h>
#include <usart.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <stdlib.h>

#define __DELAY_BACKWARD_COMPATIBLE__  
#define DELAY 350
#define ALL_LEDS 0b00111100
#define BUTTON_1 0
#define BUTTON_2 1
#define BUTTON_3 2
#define S1 PC1
#define S2 PC2
#define S3 PC3
#define MAX_LED_INDEX 3

bool gameStarted = false;
bool gameLost = false;
int flashCounter = 0;

ISR( PCINT1_vect )
{   
    // Exercise 1
    // The first time it enters the ISR() it starts the game (since the ISR() is activated from a button press).
    if(gameStarted == false) {
      gameStarted = true;
    }
}

void generatePuzzle(uint8_t array[], int size) {
  int upper = 2;
  int lower = 0;
  printf("\n\nGenerating puzzle : ");
  for (int i = 0; i < size; i++) {
    array[i] = (rand() % (upper - lower + 1)) + lower;
  }
}

void printPuzzle(uint8_t array[], int size) {
  for (int i = 0; i < size; i++) {
    if (i == 0) {
      printf("[ %d", array[i]);
    } else if (i == size-1) {
      printf(" %d ]\n", array[i]);
    } else {
      printf(" %d",array[i]);
    }
  }
}

void playPuzzle(uint8_t array[], int size) {
  _delay_ms(2000);

  for (int i = 0; i < size; i++) {
    ledOnAndOffWithDelay(MAX_LED_INDEX - array[i], DELAY); // Inverting the LED's so that first button would correspond to the top LED.
  }
}

void moveStatus(int arrayValue, int userArrayValue) {
  if (arrayValue == userArrayValue) {
    printf("\nYou pressed button %d, correct!", userArrayValue+1);
  } else {
    printf("\nYou pressed button %d, wrong!", userArrayValue+1);
    gameLost = true;
  }
}

void printPattern(uint8_t array[], uint8_t userArray[], int size) {
  printf("\nWrong, the correct pattern was : [");

  for (int i = 0; i < size; i++) {
    printf(" %d", array[i]);
  }
  printf(" ] ");

  printf("You entered : [");

  for (int i = 0; i < size; i++) {
    printf(" %d", userArray[i]);
  }
  printf(" ]\nGame over! Thank you for playing.");
}

bool readInput(uint8_t array[], int size) { // I tried to format the spaces and it did some weird stuff...
  uint8_t userInputArray[size];

  for (int i = 0; i < size; i++) {
    bool choiceMade = false;

    while (choiceMade == false ) { 
      if (buttonPushed(BUTTON_1) == 1) {
        _delay_us(1000);
        if (buttonPushed(BUTTON_1) == 1) {
          userInputArray[i] = 0;
          // printf("Comparing %d to %d", array[i], userInputArray[i]);
          moveStatus(array[i], userInputArray[i]);
          choiceMade = true;
        }
      } else if (buttonPushed(BUTTON_2) == 1) {
        _delay_us(1000);
          if (buttonPushed(BUTTON_2) == 1) {
            userInputArray[i] = 1;
            // printf("Comparing %d to %d", array[i], userInputArray[i]);
            moveStatus(array[i], userInputArray[i]);
            choiceMade = true;
          }
      } else if (buttonPushed(BUTTON_3) == 1) {
        _delay_us(1000);
          if (buttonPushed(BUTTON_3) == 1) {
            userInputArray[i] = 2;
            // printf("Comparing %d to %d", array[i], userInputArray[i]);
            moveStatus(array[i], userInputArray[i]);
            choiceMade = true;
          }
        }
    }
    _delay_ms(500); // Delay so that the button press wouldn't get registered multiple times. 
                   // I could've done this with system time, but "I think" I remember you saying that with the ATMEGA328 it's not possible.
  }

  if (gameLost == false && size < 10) {
    printf("\n\nCorrect, we go to level %d.", size+1);
    flashingLed(3, 10, 150);
    return false;
  } else if(gameLost == true) {
    printPattern(array, userInputArray, size);
    return true;
  } return false;
}


int main () {
  initUSART();
  enableLeds(ALL_LEDS);
  lightDownLeds(ALL_LEDS);
  enableAllButtons();
  PCICR |= _BV( PCIE1 );
  PCMSK1 |= _BV(S1) | _BV(S2) |_BV(S3); // Make a function for these 2, maybe. ( IF CODE NOT WORKING, CHANGE THIS TO INDIVIDUAL )

  printf("\nPress any button to start the game.");

  sei();
  while(gameStarted==false) {
    ledOnAndOffWithDelay(3, 300);
    flashCounter++;
  }

  // Exercise 2
  srand(flashCounter);
  int testArray[10];
  int upper = 100;
  int lower = 1;
  
  printf("\n\nTesting the random generator seeding : ");
  for (int i = 0; i<10; i++) {
    int num = (rand() % (upper - lower + 1)) + lower;
    testArray[i] = num;
    printf(" %d", testArray[i]);
  }

  uint8_t arrayOfTen[10];
  generatePuzzle(arrayOfTen, sizeof(arrayOfTen));
  printPuzzle(arrayOfTen, sizeof(arrayOfTen));  
  
  // Actual gameplay.
  for (int i = 1; i <= sizeof(arrayOfTen); i++) {
    playPuzzle(arrayOfTen, i); 
    readInput(arrayOfTen, i);
      if (gameLost == true) {
        lightUpLeds(ALL_LEDS);
        break;
      } else if (i<sizeof(arrayOfTen)) {
        flashingLed(4, 10, 200);
      } else {
        flashingLed(4, 10, 200);
        printf("Congratulations, you are the Simon Master!");
        knightriderLedTheme();
      }
  }
    
  return 0;
}