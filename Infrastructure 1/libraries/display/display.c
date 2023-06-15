#define __DELAY_BACKWARD_COMPATIBLE__

#include <display.h>
#include <stdbool.h>
#include <avr/io.h>
#include <util/delay.h>

#define SPACE 0xFF


// In display.c add an array with the 26 letters of the alphabet at the top of the declaration.
const uint8_t ALPHABET_MAP[] = {0x88, 0x83, 0xC6, 0xA1, 0x86, 0x8E, 0xC2,
                                0x89, 0xCF, 0xE1, 0x8A, 0xC7, 0xEA, 0xC8,
                                0xC0, 0x8C, 0x4A, 0xCC, 0x92, 0x87, 0xC1,
                                0xC1, 0xD5, 0x89, 0x91, 0xA4};

/* Segment byte maps for numbers 0 to 9 */
const uint8_t SEGMENT_MAP[] = {0xC0, 0xF9, 0xA4, 0xB0, 0x99,
                               0x92, 0x82, 0xF8, 0X80, 0X90};

// Slot machine symbols
const uint8_t SYMBOL_MAP[] = {0xBF, 0xF6, 0xB6, 0xAD, 0x9B, 0x9C, 0xA3, 0xAB, 0xAA, 0xC9};

/* Byte maps to select digit 1 to 4 */
const uint8_t SEGMENT_SELECT[] = {0xF1, 0xF2, 0xF4, 0xF8};

void initDisplay() {
  sbi(DDRD, LATCH_DIO);
  sbi(DDRD, CLK_DIO);
  sbi(DDRB, DATA_DIO);
}

void disableDisplay() {
  clearDisplay();
  cbi(DDRD, LATCH_DIO);
  cbi(DDRD, CLK_DIO);
  cbi(DDRB, DATA_DIO);
}

// loop through seven segments of LED display and shift the correct bits in the
// data register
void shift(uint8_t val, uint8_t bitorder) {
  uint8_t bit;

  for (uint8_t i = 0; i < NUMBER_OF_SEGMENTS; i++) {
    if (bitorder == LSBFIRST) {
      bit = !!(val & (1 << i));
    } else {
      bit = !!(val & (1 << (7 - i)));
    }
    // write bit to register
    if (bit == HIGH)
      sbi(PORTB, DATA_DIO);
    else
      cbi(PORTB, DATA_DIO);

    // Trigger the clock pin so the display updates
    sbi(PORTD, CLK_DIO);
    cbi(PORTD, CLK_DIO);
  }
}

//Writes a digit to a certain segment. Segment 0 is the leftmost.
void writeNumberToSegment(uint8_t segment, uint8_t value) {
  cbi(PORTD, LATCH_DIO);
  shift(SEGMENT_MAP[value], MSBFIRST);
  shift(SEGMENT_SELECT[segment], MSBFIRST);
  sbi(PORTD, LATCH_DIO);
}

//Writes a nuber between 0 and 9999 to the display. To be used in a loop...
void writeNumber(int number) {
  if (number < 0 || number > 9999) return;
  writeNumberToSegment(0, number / 1000);
  writeNumberToSegment(1, (number / 100) % 10);
  writeNumberToSegment(2, (number / 10) % 10);
  writeNumberToSegment(3, number % 10);
}

//Writes a number between 0 and 9999 to the display and makes sure that it stays there a certain number of millisecs.
//Note: the timing is approximate; the amount of time writeNumberToSegment takes is not accounted for...
void writeNumberAndWait(int number, int delay) {
  if (number < 0 || number > 9999) return;
  for (int i = 0; i < delay / 20; i++) {
    writeNumberToSegment(0, number / 1000);
    _delay_ms(5);
    writeNumberToSegment(1, (number / 100) % 10);
    _delay_ms(5);
    writeNumberToSegment(2, (number / 10) % 10);
    _delay_ms(5);
    writeNumberToSegment(3, number % 10);
    _delay_ms(5);
  }
}

void writeCharToSegment(uint8_t segment, char character) {
  cbi(PORTD, LATCH_DIO);

  if (character<0) return;

  if (character >=97 && character<=122) { // -65 to match ALPHABET_MAP 1:1.
    character = character - 32 - 65;
    shift(ALPHABET_MAP[character], MSBFIRST); // Throws a warning, the program still runs. From what I found, it's because a negative value could be passed as an array index.

  } else if (character>=65 && character<=90) {
    character = character - 65;
    shift(ALPHABET_MAP[character], MSBFIRST); // Throws a warning, the program still runs. From what I found, it's because a negative value could be passed as an array index.

  } else {
    shift(SPACE, MSBFIRST);
  }

  shift(SEGMENT_SELECT[segment], MSBFIRST);
  sbi(PORTD, LATCH_DIO);
}

void writeString(char* str) {
  writeCharToSegment(0, str[0]);
  writeCharToSegment(1, str[1]);
  writeCharToSegment(2, str[2]);
  writeCharToSegment(3, str[3]);
}

// Writes a letter to the display. To be used in a loop...
void writeStringAndWait(char* str, int delay) {
  for (int i = 0; i < delay / 20; i++) {
    writeCharToSegment(0, str[0]);
    _delay_ms(5);
    writeCharToSegment(1, str[1]);
    _delay_ms(5);
    writeCharToSegment(2, str[2]);
    _delay_ms(5);
    writeCharToSegment(3, str[3]);
    _delay_ms(5);
  }
}

void displayNimGUI(bool playerTurn, int remainingMatches, int move) {
  int firstDigit = remainingMatches / 10;
  int secondDigit = remainingMatches % 10;
  char player = playerTurn ? 'P' : 'C';

  writeNumberToSegment(0, move);
  writeCharToSegment(1, player);
  writeNumberToSegment(2, firstDigit);
  writeNumberToSegment(3, secondDigit);
}

void displayNimGUIWithoutPlayerAndWait(bool playerTurn, int remainingMatches, int move, int delay) { // bad name I know, but bare with me please
  int firstDigit = remainingMatches / 10;
  int secondDigit = remainingMatches % 10;

  for (int i = 0; i < delay / 20; i++) {
    writeNumberToSegment(0, move);
    _delay_ms(5);
    writeCharToSegment(1, SPACE);
    _delay_ms(5);
    writeNumberToSegment(2, firstDigit);
    _delay_ms(5);
    writeNumberToSegment(3, secondDigit);
    _delay_ms(5);
  }
}

void displayNimGUIAndWait(bool playerTurn, int remainingMatches, int move, int delay) {
  int firstDigit = remainingMatches / 10;
  int secondDigit = remainingMatches % 10;
  char player = playerTurn ? 'P' : 'C';

  for (int i = 0; i < delay / 20; i++) {
    writeNumberToSegment(0, move);
    _delay_ms(5);
    writeCharToSegment(1, player);
    _delay_ms(5);
    writeNumberToSegment(2, firstDigit);
    _delay_ms(5);
    writeNumberToSegment(3, secondDigit);
    _delay_ms(5);
  }
}

void displayNimGameOver(bool playerLost) {
  writeStringAndWait("Game", 1000);
  writeStringAndWait("Over", 1000);

  if (playerLost) {
    writeStringAndWait("Comp", 1000);
    writeStringAndWait("Wins", 1000);
  } else {
    writeStringAndWait("Plyr", 1000);
    writeStringAndWait("Wins", 1000);
  }
}

void clearDisplay() {
  writeString("////"); // cheating a little bit 
}

void displayCountdown(int number, int delay) { // IF THIS DOESN'T WORK CHANGE IT BACK TO "" CAUSE I SWAPPED IT WITHOUT TESTING
  for (int i = 0; i < delay / 20; i++) {
    writeCharToSegment(0, SPACE);
    _delay_ms(5);
    writeCharToSegment(1, SPACE);
    _delay_ms(5);
    writeCharToSegment(2, SPACE);
    _delay_ms(5);
    writeNumberToSegment(3, number);
    _delay_ms(5);
  }
}

// For the personal project
void writeSymbolToSegment(uint8_t segment, uint8_t value) {
  cbi(PORTD, LATCH_DIO);
  shift(SYMBOL_MAP[value], MSBFIRST);
  shift(SEGMENT_SELECT[segment], MSBFIRST);
  sbi(PORTD, LATCH_DIO);
}
