#define __DELAY_BACKWARD_COMPATIBLE__

#include <util/delay.h>
#include <avr/io.h>
#include <usart.h>
#include <display.h>
#include <buttons.h>
#include <stdbool.h>
#include <stdlib.h>
#include <flashing_leds.h>
#include <avr/interrupt.h>

#define FIRST_BUTTON 0
#define SECOND_BUTTON 1
#define THIRD_BUTTON 2
#define BUTTON1 PC1
#define BUTTON2 PC2
#define BUTTON3 PC3

int minNumber = 1;
int maxNumber = 3;

bool isbuttonPushed = false;
bool closeApplication = false;
bool playerStartsFirst;
bool isPlayerTurn; // just to have a different value to work with for turns, could've also done it with playerStartsFirst if wanted to.

int selectedMatches;
int remainingMatches; // = START_NUMBER; // Was used for vanilla, but because of 'Expansion' I give the value to the variable in main()
int playerTurnCount = 0;
int computerTurnCount = 0;

bool hardDifficulty = false; // For expansion.

// For Heap
int *playerArray;
int *computerArray;

ISR(PCINT1_vect) { // I could've done it without ISR() but it wouldn't have been as smooth.
  if (buttonPushed(FIRST_BUTTON) == 1 && isbuttonPushed == false) { // First interrupt for ending seed generation.
    _delay_us(1000);
    if (buttonPushed(FIRST_BUTTON) == 1 && isbuttonPushed == false) {
      isbuttonPushed = true;
    }
  }
}

void initADC() {
  ADMUX |= ( 1 << REFS0 );    // 5V reference voltage.
  ADCSRA |= ( 1 << ADPS2 ) | ( 1 << ADPS1 ) | ( 1 << ADPS0 ); // Used division factor: 128
  ADCSRA |= ( 1 << ADEN ); 
}

void allocateMemory() { 
  int tempValue = ((remainingMatches  / minNumber / 2) + 1); // Allocating just enough space for maximum amount of moves for player.
  playerArray = malloc(sizeof(int*) * tempValue * 2);          
  computerArray = malloc(sizeof(int*) * tempValue * 2);
}

void printWhoStartsFirst(bool value) {
  printf(value ? "\n\nPlayer starts first." : "\n\nComputer starts first.");
}

void flashIndicator() {
  for (int i = 0; i < 4; i++) {
    displayNimGUIAndWait(isPlayerTurn, remainingMatches, selectedMatches, 100);
    displayNimGUIWithoutPlayerAndWait(isPlayerTurn, remainingMatches, selectedMatches, 100);
  } 
}

int increaseSelectedMatches(int matches) {
  if (matches<maxNumber) {
    matches++;
  }
  return matches;
}

int decreaseSelectedMatches(int matches) {
  if (matches>minNumber) {
    matches--;
  }
  return matches;
}

void saveTurnInfo(int i) {
  if (isPlayerTurn) {
    playerArray[i * 2] = selectedMatches;
    playerArray[i * 2 + 1] = remainingMatches;
  } else {
    computerArray[i * 2] = selectedMatches;    
    computerArray[i * 2 + 1] = remainingMatches;
  }
}

void playerTurn() {
  selectedMatches = maxNumber;
  flashIndicator();

  while(1) {
    displayNimGUI(isPlayerTurn, remainingMatches, selectedMatches);

    if(buttonPushed(FIRST_BUTTON)) {
      displayNimGUIAndWait(isPlayerTurn, remainingMatches, selectedMatches, 300); // Acts as a delay so that one button press wouldn't get registered multiple times.
      selectedMatches = decreaseSelectedMatches(selectedMatches);

    } else if(buttonPushed(THIRD_BUTTON)) {
      displayNimGUIAndWait(isPlayerTurn, remainingMatches, selectedMatches, 300);
      selectedMatches = increaseSelectedMatches(selectedMatches);

    } else if(buttonPushed(SECOND_BUTTON)) {
      displayNimGUIAndWait(isPlayerTurn, remainingMatches, selectedMatches, 300);
      remainingMatches -= selectedMatches;
      saveTurnInfo(playerTurnCount);
      playerTurnCount++;
      isPlayerTurn = false;
      break;
    }
  }
}


void computerTurn() { // Computer always makes opposite turn, why ?? e.g. player picks 3, computer picks 1 // player picks 2, computer picks 2
  selectedMatches = (remainingMatches - 1) %  (maxNumber + 1);

  if (selectedMatches == 0) {
    selectedMatches = (rand() % (maxNumber - minNumber + 1)) + minNumber;
  }

  flashIndicator();

  while (1) {
    displayNimGUI(isPlayerTurn, remainingMatches, selectedMatches);

    if(buttonPushed(SECOND_BUTTON)) {
      displayNimGUIAndWait(isPlayerTurn, remainingMatches, selectedMatches, 300);
      remainingMatches -= selectedMatches;
      saveTurnInfo(computerTurnCount);
      computerTurnCount++;
      isPlayerTurn = true;
      break;
    }
  }
}

void printGameResult() {
  int playerSum = 0;
  int computerSum = 0;

  for (int d = 0; d < playerTurnCount; d++) {
    printf("\nPlayer burned %d matches in round %d. Matches left - %d.", playerArray[d * 2], d+1, playerArray[d * 2 + 1]);
    playerSum+=playerArray[d * 2]; 
  }

  printf("\n");

  for (int n = 0; n < computerTurnCount; n++) {
    printf("\nComputer burned %d matches in round %d. Matches left - %d.", computerArray[n * 2], n+1, computerArray[n * 2 + 1]);
    computerSum+=computerArray[n * 2];
  }

  printf("\n\nPlayer burned a total of %d matches.", playerSum);
  printf("\n\nComputer burned a total of %d matches.", computerSum);
}

void generateWhoStartsFirst(int value) {
  printf("The seed value is: %d", value); // TODO: 1 - 9999 instead of 1 - 1023
  playerStartsFirst = rand() % 2 == 1 ? true : false;
  isPlayerTurn = playerStartsFirst;
  printWhoStartsFirst(playerStartsFirst);
}

int main() { // NOTE: I could've done a constant flashing letter but I chose not to, flashIndicator = proof. The game would look better without it IMO. 
            // Also, I'm sorry, but I couldn't understand what was written in 'Expansion'.
  initUSART();
  initADC();
  initDisplay();
  enableAllButtons();

  PCICR |= _BV(PCIE1);
  PCMSK1 |= _BV(BUTTON1) | _BV(BUTTON2) | _BV(BUTTON3);
  sei();

  // Start of the game.
  // Generating seed for the random generator until the user presses the first button.
  uint16_t value;

  printf("\nStarting game.\n");

  while(isbuttonPushed == false) {
    ADCSRA |= ( 1 << ADSC );
    loop_until_bit_is_clear( ADCSRA, ADSC );
    value = ADC * 9.774193548387097; // This theoretically counts as a value 'between' 0 and 9999
    writeNumberAndWait(value, 250); // just to view what seed is being used
  }

  cli();

  srand(value);
  generateWhoStartsFirst(value);

  // EXPANSION. But the provided formula for AI selecting pieces doesn't work with different values.
  if (hardDifficulty) {
    minNumber *= 3;
    maxNumber *= 3;
    if (value % 1000 / 10 < 21) {
      remainingMatches = 21;
    } else {
      remainingMatches = value % 1000 / 10;
    }

  } else {
    remainingMatches = 21;
  }
  
  allocateMemory();

  // Game starts.
  while(remainingMatches != 1) {
    if(isPlayerTurn) {
      playerTurn();
    } else if (!isPlayerTurn){
      computerTurn();
    }
  }

  // Game over.
  isPlayerTurn ? printf("\n\nComputer won, thank you for playing!\n") : printf("\n\nPlayer won, congratulations!\n");
  printGameResult();

  while(1) {
    displayNimGameOver(isPlayerTurn); 
  }

  return 0;
}