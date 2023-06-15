#define __DELAY_BACKWARD_COMPATIBLE__

#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <usart.h>
#include <string.h> // remove

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

#define SCROLLING_STRING(string) "     "string"    "

// Timer related
#define MULTIPLE 250
uint32_t counter;

// Starting values
#define STARTING_DISTANCE 9999
#define STARTING_FUEL 1500
#define STARTING_SPEED 100
#define GRAVITY 1.622 // acceleration in m/s²

// Fuel related
#define FUEL_LEVEL_3 (1500 / 4) * 3
#define FUEL_LEVEL_2 (1500 / 4) * 2
#define FUEL_LEVEL_1 (1500 / 4)

// LED related
#define UPDATE_INTERVAL 1000
uint8_t allLeds = 0b00111100;
uint8_t threeLeds = 0b00011100;
uint8_t twoLeds = 0b00001100;
uint8_t oneLed = 0b00000100;
uint8_t noLed = 0b00000000;
bool ledOn = false;

// For LED flash intensity
bool speedOne = false;
bool speedTwo = false;
bool speedThree = false;
bool speedFour = false;
bool fuelWarning = false;
bool soundOn;


// Game values
int distance = STARTING_DISTANCE; // distance to the lunar surface in meters - m
int fuelReserve = 1500; // liter
int currentSpeed = 100; // speed in m/s (meters per second)
double gravity = 1.622; // acceleration in m/s²
int burst = 0;
volatile bool gameStarted = false;
bool burstActivated = false;
bool gameWon = false;
bool gameOver = false;

// Timer
int minutes;
int seconds;

// Struct & co
int structArrayLength = 10; // In a real scenario I would have this at 100, but for the sake of showing it works I did 10.
int dataLoggedCount = 0;

typedef struct {
  int* distance;
  int* currentSpeed;
  int* burst;
  int* fuelReserve;
} GAMEDATA;

GAMEDATA* gameLog;


void initTimer() {    
  TCCR2A |= _BV(WGM21); 

  TIMSK2 |= _BV(OCIE2A); 

  OCR2A = 249;

  sei();
}

void startTimer() {    
    TCCR2B |= _BV(CS22) | _BV(CS21);
}

void stopTimer() {    
    TCCR2B &= ~(_BV(CS22) | _BV(CS21)); 
}

void verifyMalloc() {
  if (gameLog != NULL) {
    printf("\n\nMemory sucessfully allocated.\n");
  } else {
    printf("\nMemory allocation failed. Programmer, check the code again!");
  }
}

void allocateMemoryForStruct() {
  gameLog->distance = malloc(sizeof(int) * structArrayLength);
  gameLog->currentSpeed = malloc(sizeof(int) * structArrayLength);
  gameLog->burst = malloc(sizeof(int) * structArrayLength);
  gameLog->fuelReserve = malloc(sizeof(int) * structArrayLength);

  verifyMalloc(gameLog);
}

 void reallocateMemoryForStruct() {
  // printf("\n\nReallocating more memory.\nFrom %d to %d.\n", structArrayLength, structArrayLength*2);
  structArrayLength = structArrayLength * 2;
  // printf("\nValue now : %d.", structArrayLength);
  gameLog->distance = realloc(gameLog->distance, sizeof(int) * structArrayLength);
  gameLog->currentSpeed = realloc(gameLog->currentSpeed, sizeof(int) * structArrayLength);
  gameLog->burst = realloc(gameLog->burst, sizeof(int) * structArrayLength);
  gameLog->fuelReserve = realloc(gameLog->fuelReserve, sizeof(int) * structArrayLength);

  verifyMalloc(gameLog);
 }

 void saveGameData() {
   if (dataLoggedCount == structArrayLength) {
     reallocateMemoryForStruct(gameLog);
   }

   gameLog->distance[dataLoggedCount] = distance;
   gameLog->currentSpeed[dataLoggedCount] = currentSpeed;
   gameLog->burst[dataLoggedCount] = burst;
   gameLog->fuelReserve[dataLoggedCount] = fuelReserve;
   printf("\nLOG #%d: Distance - %d ; Current Speed - %d ; Burst - %d ; Fuel Reserve - %d.", 
   dataLoggedCount ,gameLog->distance[dataLoggedCount], gameLog->currentSpeed[dataLoggedCount], gameLog->burst[dataLoggedCount], gameLog->fuelReserve[dataLoggedCount]);
 }

void ledsWithDisplay(uint8_t leds) {
  lightDownAllLeds();
  if (ledOn) {
    lightUpLeds(leds);
  } else {
    lightDownLeds(leds);
  }
}

// void warningSound() {
//   if (soundOn) {
//     enableBuzzer();
//   } else {
//     disableBuzzer();
//   }
// }

void showParameters() { 
  int localDistance;
  soundOn = false;
  speedOne = false;
  speedTwo = false;
  speedThree = false;
  speedFour = false;
    
  if (distance <= 9999) { // the whole program bugs out above 9999 distance.
    localDistance = distance;
  } else {
    localDistance = 9999;
  }

  if (fuelReserve == 1500) {
    lightUpAllLeds();
    writeNumber(localDistance);
  } else if (fuelReserve <= 0) {
    // fuelWarning = true;
    // warningSound();
    lightDownAllLeds();
    writeNumber(localDistance);
  } else if (fuelReserve < FUEL_LEVEL_1) {
    speedOne = true;
    ledsWithDisplay(oneLed);
    writeNumber(localDistance);
  } else if (fuelReserve < FUEL_LEVEL_2) {
    speedTwo = true;
    ledsWithDisplay(twoLeds);
    writeNumber(localDistance);
  } else if (fuelReserve < FUEL_LEVEL_3) {
    speedThree = true;
    ledsWithDisplay(threeLeds);
    writeNumber(localDistance);
  } else if (fuelReserve > FUEL_LEVEL_3) {
    speedFour = true;
    ledsWithDisplay(allLeds);
    writeNumber(localDistance);
  }
}

ISR(PCINT1_vect) {
  // If any button pressed & game started == false
  while (gameStarted == false) {
    if (buttonPushed(FIRST_BUTTON) == 1 || buttonPushed(SECOND_BUTTON) || buttonPushed(THIRD_BUTTON) == 1) {
      _delay_us(1000);
      if (buttonPushed(FIRST_BUTTON) == 1 || buttonPushed(SECOND_BUTTON) || buttonPushed(THIRD_BUTTON) == 1) {
        gameStarted = true;
      }
    }
  }

  if (gameStarted == true) {
    if (buttonPushed(SECOND_BUTTON) == 1) {
      _delay_us(1000);
      if (buttonPushed(SECOND_BUTTON) == 1) {
        burstActivated = true;
      }

    } else if (buttonPushed(SECOND_BUTTON) == 0) {
      _delay_us(1000);
      if (buttonPushed(SECOND_BUTTON) == 0) {
        burstActivated = false;
      } 
    }
  }
}

void burstValue() { // implemented different values for burst just cause it's more fun.
  if (fuelReserve > 0 && burstActivated == true) {
    if (burst < 49) {
      burst += 2;
    }
    
  } else if (burstActivated == false) {
    if (burst > 0) {
      burst--;
    }
  }
}

void calculateNewSituation() { // TODO: Fuel can go below 0 probably something to do with my burst --, maybe remove that
  currentSpeed += GRAVITY - burst / 5;
  distance -= currentSpeed;
  fuelReserve -= burst;
}

ISR(TIMER2_COMPA_vect) {
  if ((counter+1) % MULTIPLE == 0) {
    calculateNewSituation();
    showParameters(distance, fuelReserve);
    saveGameData();
    dataLoggedCount++;
  }

  // if ((counter+1) % 125 == 0 && fuelWarning == true) {
  //   soundOn = !soundOn;
  // }

  if ((counter+1) % 250 == 0 && speedFour == true) { // Every 1 second.
    ledOn = !ledOn;
  } 

  if ((counter+1) % 125 == 0 && speedThree == true) { // Every 500ms
    ledOn = !ledOn;
  }

  if ((counter+1) % 50 == 0 && speedTwo == true) {
    ledOn = !ledOn;
  }

  if ((counter+1) % 25 == 0 && speedOne == true) {
    ledOn = !ledOn;
  }

  if ((counter+1) % 50 == 0) {
    burstValue();
  }
  counter++;
}

void countdown() {
  for (int i = 5; i > 0; i--) {
    displayCountdown(i, 1000);
  }

  writeStringAndWait(" GO ", 1000);
}

void scrollingText(char* name) {
  for (int i = 0; i < strlen(name)-4; i++) {
    if (gameStarted == true && gameOver == false) {
      break;
    }
    writeStringAndWait(&name[i], 200); 
  }
}

void victory() {
  printf("\n\nCongratulations, the Lunar Lander has landed safely.\nThat's one small step for man, one giant leap for mankind.");
  int frequency[] = {240, 240, 240, 360};
  int duration[] = {100, 100, 100, 600};
  int delay[] = {100, 100, 100, 100};
  enableBuzzer();

  for (int i = 0; i < sizeof(frequency) / sizeof(int); i++) {
    playTone(frequency[i], duration[i]);
    _delay_ms(delay[i]);
  }
  disableBuzzer();

  gameWon = true;
  gameOver = true;
}

void defeat() {
  printf("\n\nHouston, we've had a problem...\nGame over!");
  int frequency[] = {140, 100, 80, 60};
  int duration[] = {360, 360, 360, 1000};
  int delay[] = {360, 360, 360, 360};
  enableBuzzer();

  for (int i = 0; i < sizeof(frequency) / sizeof(int); i++) {
    playTone(frequency[i], duration[i]);
    _delay_ms(delay[i]);
  }
  disableBuzzer();

  gameWon = false;
  gameOver = true;
}

void endGameLog() {
  printf("\n\nSpeed at the moment of landing: %d.\nFuel left: %d.", currentSpeed, fuelReserve);
  printf("\n\nGAME LOG\n=================");
  for (int i = 0; i < dataLoggedCount; i++) {
    printf("\nLOG #%d: Distance - %d ; Current Speed - %d ; Burst - %d ; Fuel Reserve - %d.", 
   i ,gameLog->distance[i], gameLog->currentSpeed[i], gameLog->burst[i], gameLog->fuelReserve[i]);
  }
}



int main() {
  initUSART();    
  initDisplay();
  initTimer();
  enableAllButtons();

  PCICR |= _BV(PCIE1);
  PCMSK1 |= _BV(BUTTON1) | _BV(BUTTON2) | _BV(BUTTON3);
  sei();

  gameLog = malloc(sizeof(GAMEDATA));
  allocateMemoryForStruct();

  printf("\nWelcome to Lunar Lander NG 1.0.\nPress any button to start the game.");

  while (gameStarted == false) {
    scrollingText(SCROLLING_STRING("Lunar Lander"));
  }

  countdown();
  startTimer();
  enableAllLeds();
  lightDownLeds(allLeds);

  while (distance >= 3) {
    showParameters(distance, fuelReserve);
  }

  // stopTimer();
  cli();
  
  currentSpeed <= 5 ? victory() : defeat();

  endGameLog();
  // free(gameLog);

  while(1) {
    gameWon ? scrollingText(SCROLLING_STRING("Victory")) : scrollingText(SCROLLING_STRING("Defeat"));
  }

  return 0;
}