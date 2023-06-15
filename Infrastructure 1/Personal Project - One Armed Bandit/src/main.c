#define __DELAY_BACKWARD_COMPATIBLE__
#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <usart.h>
#include <string.h>

#include <display.h>
#include <buttons.h>
#include <flashing_leds.h>
#include <buzzer.h>
#include <potentio.h>

#define BUTTON1 PC1
#define BUTTON2 PC2
#define BUTTON3 PC3

#define FIRST_BUTTON 0
#define SECOND_BUTTON 1
#define THIRD_BUTTON 2

#define SCROLLING_STRING(string) "     "string"    " // Macro for scrolling text
#define ARRAY_LENGTH(array) (sizeof(array) / sizeof(array[0])
#define COLUMN_LENGTH 4
#define MULTIPLE 1000

// Timer related
uint32_t counter;
int minutes;
int seconds;

// Initial game values
bool gameStarted = false; // using a volatile variable so that I could break out of a loop.
bool gameOver = false;
bool gameWon = false;
int coinsAvailable = 50;
int displaySize = 2;
int betSize = 0;
int coinsWon;
int roundsPlayed = 0;

// Values for controlling in which menu the player is.
bool spinningReels = false;
bool financialStatus = false;
bool soundOn = false;

// Mid game values
int offset;
int currentCombination[COLUMN_LENGTH];
uint32_t buttonPressMillis; // button press duration

// Struct for gamelog
typedef struct { // Could've done the whole program with only struct, but I only added it at the end, and didn't want to remake the whole thing.
  int* turn;
  int* coinsAvailable;
  int* betSize;
  int* displaySize;
  int* firstReel;
  int* secondReel;
  int* thirdReel;
  int* fourthReel;
  int* coinsWon; 
} GAMEDATA;

// For game logging.
GAMEDATA* gameLog;
int structArraySize = 2; // Usually this would be at 50 (minimum), but in order to show off that it works, it's set at 2.

void initTimer() {    
  TCCR2A |= _BV(WGM21); 

  TIMSK2 |= _BV(OCIE2A); 

  OCR2A = 249;

  sei();
}

void startTimer() {    
    TCCR2B |= _BV(CS22);
}

void stopTimer() {    
    TCCR2B &= ~(_BV(CS22)); 
}

void tick() {    
    // enter your code
  if (seconds + 1 == 60) {
    seconds = 0;
    minutes++;

  } else { // if it hits 59, I switch to 0, and don't add a second, so that 00 would be displayed briefly.
    seconds++;
  }
}

void initADC() {
  ADMUX |= ( 1 << REFS0 );    // 5V reference voltage.
  ADCSRA |= ( 1 << ADPS2 ) | ( 1 << ADPS1 ) | ( 1 << ADPS0 ); // Used division factor: 128
  ADCSRA |= ( 1 << ADEN ); 
}

void readPotentiometerValue(int *value) {
  ADCSRA |= ( 1 << ADSC );
  loop_until_bit_is_clear( ADCSRA, ADSC );
  *value = ADC;
}

void verifyMalloc(GAMEDATA* log) {
  if (log != NULL) {
    printf("\n\nMemory sucessfully allocated.\n");
  } else {
    printf("\nMemory allocation failed. Programmer, check the code again!");
  }
}

void allocateMemory() {
  gameLog = malloc(sizeof(GAMEDATA));
  gameLog->turn = malloc(sizeof(int) * structArraySize);
  gameLog->coinsAvailable = malloc(sizeof(int) * structArraySize);
  gameLog->betSize = malloc(sizeof(int) * structArraySize);
  gameLog->displaySize = malloc(sizeof(int) * structArraySize);
  gameLog->firstReel = malloc(sizeof(int) * structArraySize); 
  gameLog->secondReel = malloc(sizeof(int) * structArraySize);
  gameLog->thirdReel = malloc(sizeof(int) * structArraySize);
  gameLog->fourthReel = malloc(sizeof(int) * structArraySize);
  gameLog->coinsWon = malloc(sizeof(int) * structArraySize);

  verifyMalloc(gameLog);
 }

 void reallocateMemory() {
  structArraySize+=structArraySize;
  gameLog->turn = realloc(gameLog->turn, sizeof(int) * structArraySize);
  gameLog->coinsAvailable = realloc(gameLog->coinsAvailable, sizeof(int) * structArraySize);
  gameLog->betSize = realloc(gameLog->betSize, sizeof(int) * structArraySize);
  gameLog->displaySize = realloc(gameLog->displaySize, sizeof(int) * structArraySize);
  gameLog->firstReel = realloc(gameLog->firstReel, sizeof(int) * structArraySize);
  gameLog->secondReel = realloc(gameLog->secondReel, sizeof(int) * structArraySize);
  gameLog->thirdReel = realloc(gameLog->thirdReel, sizeof(int) * structArraySize);
  gameLog->fourthReel = realloc(gameLog->fourthReel, sizeof(int) * structArraySize);
  gameLog->coinsWon = realloc(gameLog->coinsWon, sizeof(int) * structArraySize);

  verifyMalloc(gameLog);
 }

 void printTurnData(int round) {
  printf("\nTurn %d: %d coins available - %d coin bet - ",
  gameLog->turn[round],
  gameLog->coinsAvailable[round],
  gameLog->betSize[round]);

  if (displaySize == 4) {
    printf("%d%d%d%d",
    gameLog->firstReel[round],
    gameLog->secondReel[round],
    gameLog->thirdReel[round],
    gameLog->fourthReel[round]);

  } else if (displaySize == 3) {
    printf("%d%d%d", 
    gameLog->secondReel[round],
    gameLog->thirdReel[round],
    gameLog->fourthReel[round]);

  } else if (displaySize == 2) {
    printf("%d%d",
    gameLog->thirdReel[round],
    gameLog->fourthReel[round]);
  }

  printf(" - %d coins won.", gameLog->coinsWon[round]);
 }

 void saveGameData() {
   if (roundsPlayed == structArraySize) { // If the heap is full, reallocate more memory.
    printf("\n\nReallocating more memory.\n");
    reallocateMemory();
   }
   
  gameLog->turn[roundsPlayed] = roundsPlayed+1;
  gameLog->coinsAvailable[roundsPlayed] = coinsAvailable+betSize;
  gameLog->betSize[roundsPlayed] = betSize;
  gameLog->displaySize[roundsPlayed] = displaySize;
  gameLog->firstReel[roundsPlayed] = currentCombination[0];
  gameLog->secondReel[roundsPlayed] = currentCombination[1];
  gameLog->thirdReel[roundsPlayed] = currentCombination[2];
  gameLog->fourthReel[roundsPlayed] = currentCombination[3];
  gameLog->coinsWon[roundsPlayed] = coinsWon;

  printTurnData(roundsPlayed);
 }

void scrollingText(char* name) {
  for (int i = 0; i < strlen(name)-4; i++) {
    if (gameStarted == true && gameOver == false) {
      break;
    } else {
      writeStringAndWait(&name[i], 200); 
    }
  }
}

void displayBetSize(int betSize) {
  for (int i = 0; i < betSize; i++) {
    lightUpLed(i);
  }
}

void resetBetSize() {
  betSize = 0;
  lightDownAllLeds();
}

void getOffset() {
  if (displaySize == 4) {
    offset = 0;
  } else {
    offset = 1;
  }
}

void displayReel() {
  getOffset();

  for (int i = offset; i < displaySize + offset; i++) {
    writeSymbolToSegment(i, currentCombination[i]);
    displayBetSize(betSize);
  }
}

void generateReelCombination(int* array) {
  for (int i = 0; i < 4; i++) {
    array[i] = (rand() % (9 - 0 + 1) + 0);
  }
}

void writeCombinationAndWait(int firstReel, int secondReel, int thirdReel, int fourthReel, int delay) {
  if (displaySize == 4) {
    for (int i = 0; i < delay / 20; i++) {
      writeSymbolToSegment(0, firstReel);
      _delay_ms(5);
      writeSymbolToSegment(1, secondReel);
      _delay_ms(5);
      writeSymbolToSegment(2, thirdReel);
      _delay_ms(5);
      writeSymbolToSegment(3, fourthReel);
      _delay_ms(5);
    }
  } else if (displaySize == 3) {
    for (int i = 0; i < delay / 15; i++) {
      writeSymbolToSegment(1, secondReel);
      _delay_ms(5);
      writeSymbolToSegment(2, thirdReel);
      _delay_ms(5);
      writeSymbolToSegment(3, fourthReel);
      _delay_ms(5);
    }
  } else if (displaySize == 2) {
    for (int i = 0; i < delay / 10; i++) {
      writeSymbolToSegment(1, secondReel);
      _delay_ms(5);
      writeSymbolToSegment(2, thirdReel);
      _delay_ms(5);
    }
  }
}

void playSound(int* frequency, int* duration, int* delay, int length) {
  if (gameOver) {
    stopTimer();
    gameWon ? printf("\n\nCongratulations, you've bankrupted the casino.") : printf("\n\nSorry, you're out of funds. We'll be waiting your next visit...");
  }

  soundOn = true;
  enableBuzzer();
  for (int i = 0; i < length; i++) {
    playTone(frequency[i], duration[i]);
    _delay_ms(delay[i]);
  }
  disableBuzzer();
  soundOn = false;
}

void gameWinSound() {
  int frequency[] = {523, 523, 523, 523, 415, 466, 523, 0, 477, 523};
  int duration[] = {66, 66, 66, 200, 200, 200, 66, 0, 66, 600};
  int delay[] = {66, 66, 66, 200, 200, 200, 66, 66, 66, 600};

  playSound(frequency, duration, delay, ARRAY_LENGTH(frequency)));
}

void gameLoseSound() {
  int frequency[] = {140, 100, 80, 60};
  int duration[] = {360, 360, 360, 1000};
  int delay[] = {360, 360, 360, 360};

  playSound(frequency, duration, delay, ARRAY_LENGTH(frequency)));
}

void spinWinSound() { // doesn't work that well cause looping introduces delays, but whatever.
  int frequency[] = {240, 240, 240, 360};
  int duration[] = {50, 50, 50, 400};
  int delay[] = {50, 50, 50, 50};

  playSound(frequency, duration, delay, ARRAY_LENGTH(frequency)));
}

void checkCoinAmount() {
  if (coinsAvailable <= 0) {
    gameOver = true;
    gameWon = false;
    gameLoseSound();
  } else if (coinsAvailable >= 9999) {
    gameOver = true;
    gameWon = true;
    gameWinSound();
  }
}

void checkWinnings() {
  coinsWon = 0;
  if (displaySize == 4) {
    if (currentCombination[0] == currentCombination[1] && currentCombination[1] == currentCombination[2] && currentCombination[2] == currentCombination[3]) {
      coinsWon = betSize * 500;
      coinsAvailable += coinsWon;
      printf("\nCongratulations, you have won %d coins.", coinsWon);
      spinWinSound();
    } else {
      coinsAvailable -= betSize;
    }
  }

  if (displaySize == 3) {
    if(currentCombination[1] == currentCombination[2] && currentCombination[2] == currentCombination[3]) {
      coinsWon = betSize * 50;
      coinsAvailable += coinsWon;
      printf("\nCongratulations, you have won %d coins.", coinsWon);
      spinWinSound();
    } else {
      coinsAvailable -= betSize;
    }
  }

  if (displaySize == 2) {
    if(currentCombination[1] == currentCombination[2]) {
      coinsWon = betSize * 5;
      coinsAvailable += coinsWon;
      printf("\nCongratulations, you have won %d coins.", coinsWon);
      spinWinSound();
    } else {
      coinsAvailable -= betSize;
    }
  }
  // Add data to heap and reset.
  saveGameData();
}

void reelStopping(int i) { // There's a really really small flicker when this gets called but fixing this micro issue is not worth it. 
                          // Could do it by remaking the writeCombinationAndWait & replacing the last 5ms delay with a buzzing sound when necessary.
  if (i == 19 || i == 29 || i == 39 || i == 49) {
    enableBuzzer();
    playTone(80, 5);
    disableBuzzer();
  }
}

void spinSlot() {
  spinningReels = true;
  int temporaryCombination[COLUMN_LENGTH];
  generateReelCombination(currentCombination);
  // I know that the repetive loops are ugly and unconventional, but I can't replace them, at least in this state.
  if (displaySize == 4) {
    for (int i = 0; i < 50; i++) {
      generateReelCombination(temporaryCombination);
      if (i < 20) {
        writeCombinationAndWait(temporaryCombination[0], temporaryCombination[1], temporaryCombination[2], temporaryCombination[3], 100);   
        reelStopping(i);
      } else if (i < 30) {
        writeCombinationAndWait(currentCombination[0], temporaryCombination[1], temporaryCombination[2], temporaryCombination[3], 100);
        reelStopping(i);
      } else if (i < 40) {
        writeCombinationAndWait(currentCombination[0], currentCombination[1], temporaryCombination[2], temporaryCombination[3], 100);
        reelStopping(i);
      } else {
        writeCombinationAndWait(currentCombination[0], currentCombination[1], currentCombination[2], temporaryCombination[3], 100);      
        reelStopping(i);
      }
    }
  } 

  if (displaySize == 3) {
    for (int i = 0; i < 40; i++) {
      generateReelCombination(temporaryCombination);
      if (i < 20) {
        writeCombinationAndWait(temporaryCombination[0], temporaryCombination[1], temporaryCombination[2], temporaryCombination[3], 100);     
        reelStopping(i);   
      } else if (i < 30) {
        writeCombinationAndWait(currentCombination[0], currentCombination[1], temporaryCombination[2], temporaryCombination[3], 100);
        reelStopping(i);
      } else {
        writeCombinationAndWait(currentCombination[0], currentCombination[1], currentCombination[2], temporaryCombination[3], 100);
        reelStopping(i);
      }
    }
  } 
  
  if (displaySize == 2) {
    for (int i = 0; i < 30; i++) {
      generateReelCombination(temporaryCombination);
      if (i < 20) {
        writeCombinationAndWait(temporaryCombination[0], temporaryCombination[1], temporaryCombination[2], temporaryCombination[3], 100);    
        reelStopping(i);    
      } else if (i < 30) {
        writeCombinationAndWait(currentCombination[0], currentCombination[1], temporaryCombination[2], temporaryCombination[3], 100);
        reelStopping(i);
      } 
    }
  }
  spinningReels = false;
  checkWinnings();
  resetBetSize();
  checkCoinAmount(); // Checks if user has any funds left or if it's reached the 9999 mark.
  roundsPlayed++;
}

void buttonPressDelay() {
  displayBetSize(betSize);
  writeCombinationAndWait(currentCombination[0], currentCombination[1], currentCombination[2], currentCombination[3], 200);
}

void printGameLog(int valueOne, int *valueTwo) { 
  valueOne = 2021; // copy of the original value, so won't be changed
  *valueTwo = 2021; // value references the original data, so will be changed

  int timesWon = 0;

  printf("\n\nPrinting game log.\n===================");
  
  for (int i = 0; i < roundsPlayed; i++) {
    printTurnData(i);
  }

  printf("\n\n===================");
  
  for (int j = 0; j < roundsPlayed; j++) {
    if (gameLog->coinsWon[j] != 0) {
      timesWon++;
    }
  }
  
  int winRate = (timesWon*100)/100; // this would be the correct one ((float)timesWon/roundsPlayed) * 100.0; but this thing can't display float values.
  // I didn't save these values in the heap, but I can/could do that if necessary.
  printf("\nRounds played: %d.\nTimes Won: %d.\nWin Ratio: %d%%.\nTime Played: %d minutes and %d seconds.\n===================",
  roundsPlayed, timesWon, winRate, minutes, seconds);
  gameOver = true;
}

void playGame() {
  if (gameStarted == true && spinningReels == false && financialStatus == false && gameOver == false) {
    if (buttonPushed(SECOND_BUTTON) == 1) {
      _delay_us(1000);
      if (buttonPushed(SECOND_BUTTON) == 1) {
        buttonPressMillis = 0;
        while (buttonPushed(SECOND_BUTTON) == 1) {
          _delay_ms(1);
          displayReel();
          buttonPressMillis++;
        }
        if (buttonPressMillis < 500 && betSize > 0) {
          if (coinsAvailable - betSize >= 0) {
            // printf("\nSpinning the slot!");
            spinSlot();
          } else {
            // printf("Sorry you don't have enough funds, lower your bet please.");
          }
          
        } else if (buttonPressMillis >= 500) {
          // printf("\nOpening menu");
          financialStatus = true;
          buttonPressDelay();
        }
      }
    }

    if (buttonPushed(FIRST_BUTTON) == 1) {
      _delay_us(1000);
      if (buttonPushed(FIRST_BUTTON) == 1) {
        if (displaySize + 1 == 5) {
          // printf("\nValue would be too high. Setting it as min value.");
          displaySize = 2;
          buttonPressDelay();
        } else {
          // printf("\nIncreasing display count.");
          displaySize++;
          buttonPressDelay();
        }
      }
    }

    if (buttonPushed(THIRD_BUTTON) == 1) {
      _delay_us(1000);
      if (buttonPushed(THIRD_BUTTON) == 1) {
        if (betSize + 1 == 5) {
          // printf("\nMaximum bet amount reached.");
          lightDownAllLeds();
          betSize = 1;
          buttonPressDelay();
        } else if (betSize + 1 > coinsAvailable) {
          // printf("Sorry. You don't have enough coins for this bet size.");
          lightDownAllLeds();
          betSize = 1;
          buttonPressDelay();
        } else {
          // printf("\nIncreasing bet amount.");
          lightDownAllLeds();
          betSize++;
          buttonPressDelay();
        }
      }
    }
  }

  if (financialStatus == true) {
    if (buttonPushed(SECOND_BUTTON) == 1) {
      _delay_us(1000);
      if (buttonPushed(SECOND_BUTTON) == 1) {
        // printf("\nReturning back to the game.");
        financialStatus = false;
        buttonPressDelay();
      }
    }
  }
}

ISR(PCINT1_vect) {
  // Pressing the middle button starts the game.
  if (gameStarted == false) {
    if (buttonPushed(SECOND_BUTTON) == 1) {
      _delay_us(1000);
      if (buttonPushed(SECOND_BUTTON) == 1) {
        _delay_ms(500);
        gameStarted = true;
      }
    }
  }
}

ISR(TIMER2_COMPA_vect) {
  if (soundOn == true) { 
    displayReel();
  }

  if ((counter+1) % MULTIPLE == 0) {
    tick();
  }

  counter++; // Every ms.
}

int main() {  
  initUSART(); 
  initADC();
  initDisplay();
  enableAllLeds();
  lightDownAllLeds();
  enableAllButtons();
  
  PCICR |= _BV(PCIE1);
  PCMSK1 |= _BV(BUTTON1) | _BV(BUTTON2) | _BV(BUTTON3);
  sei();

  int potentiometerValue;

  allocateMemory();
  printf("\nChoose the seed value by rotating the potentiometer.");

  while (gameStarted == false) {
    scrollingText(SCROLLING_STRING("One Armed Bandit"));
    readPotentiometerValue(&potentiometerValue);
  }

  printf("\n\nStarting the game. Seed value - %d.\n===================", potentiometerValue);

  srand(potentiometerValue);
  initTimer();
  startTimer();

  while (gameStarted == true && spinningReels == false && gameOver == false) {
    if (financialStatus == false) {
      displayReel();
    } else {
      writeNumber(coinsAvailable);
    }
    playGame();
  }

  // stopTimer();
  cli(); // stopping all interrupts. game ended...

  // Value/Pointer/Reference because I didn't know where else to put it, I do have a couple of use cases throughout the project as well.
  int studyYearValueOne = 2020; // value1
  int studyYearValueTwo = 2020; // value2
  int* startingYear = &studyYearValueOne; // pointer to address of value1
  int* endingYear = &studyYearValueTwo; // pointer to address of value2
  
  printGameLog(*startingYear, endingYear); // passing value and reference to function
  printf("\nThank you for playing the game.\nRodžers Ušackis %d/%d\n", studyYearValueOne, studyYearValueTwo);

  initDisplay(); // I turned it off for win/lose sounds, so I'm turning it back on for end game screen.
  while (1) {
    gameWon ? scrollingText(SCROLLING_STRING("Bank Break")) : scrollingText(SCROLLING_STRING("Game Over"));
  }

  return 0;
}