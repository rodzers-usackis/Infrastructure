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

#define MIN_ARRAY_INDEX 0
#define MAX_ARRAY_INDEX(arr) (sizeof(arr) / sizeof(arr[0]) - 1)

// Use a table with frequencies (see music tutorial for values) and a random duration (1000, 500 or 250).
// 5th octave
int noteFrequencies[] = { 523.25, 587.33, 569.25, 698.46, 783.99, 880, 987.77, 1046.5 };
int noteDuration[] = { 1000, 500, 250 };
int play = false;
int generatorSeed;

int randomNumberInRange(int min, int max) { // might create a library for random calculations if I need to use it in another project.
  return (rand() % (max - min + 1)) + min;
}

// 1.
typedef struct {
  float frequency;
  uint16_t duration;
} NOTE;

typedef struct {
  char* name;
  NOTE** notes;
  uint16_t length;
} SONG;

// 2.
void playNote (NOTE* note) {
  if (note->frequency == 0) {
    _delay_ms(note->duration);

  } else {
    playTone(note->frequency, note->duration);
  }
}

// 3.
SONG* generateSong(char* name, uint16_t length) {
  SONG* song = malloc(sizeof(SONG)); // 3.1. Allocate enough space on the HEAP to contain 1 SONG and store in SONG* song variable.
  song->length = length; // 3.2. Use the parameter to set song -> length.

  song->name = malloc(sizeof(char) * (strlen(name) + 1)); // 3.3. Allocate enough space on the HEAP to contain the name: the length of the name + 1 for the closing zero.
  strcpy(song->name, name); // 3.3. Save in song -> name. Now copy the name to song -> name.

  song->notes = malloc(sizeof(NOTE) * length); // 3.4. Allocate enough space on the HEAP to reserve an array of length number of NOTE* elements and save to song -> notes.

  for (int i = 0; i < length; i++) {
    song->notes[i] = malloc(sizeof(NOTE)); // 3.5. Allocate in a loop length times enough space to contain a NOTE and save in song -> notes[i].

    song->notes[i]->duration = noteDuration[randomNumberInRange(MIN_ARRAY_INDEX, MAX_ARRAY_INDEX(noteDuration))]; 
    song->notes[i]->frequency = noteFrequencies[randomNumberInRange(MIN_ARRAY_INDEX, MAX_ARRAY_INDEX(noteFrequencies))];
  }
  return song;
}

void playSong(SONG* song) {
  for (int i = 0; i < song->length; i++) {
    playNote(song->notes[i]);
  }
}

void resetValues() {
  play = false; // resetting the value after the song is over so that it could be played again.
  disableBuzzer();
  generatorSeed = 0; // resetting seed.
}

ISR(PCINT1_vect) {
  if (buttonPushed(FIRST_BUTTON) && play == false) {
    _delay_us(1000);
    if (buttonPushed(FIRST_BUTTON) && play == false) {
      printf("\n\nButton pressed ");
      play = true;
    }
  }
}

int main() {
  initUSART();
  enableButton(FIRST_BUTTON);

  PCICR |= _BV(PCIE1);
  PCMSK1 |= _BV(BUTTON1);
  sei();

  printf("\nStarting application.");
  
  while (1) {
    if (play == true) {
      srand(generatorSeed);
      printf("starting song generation.");
      SONG* aphexTwin = generateSong("yung aphex twin", 10);

      printf("\n\nPlaying song: %s - %d notes.", aphexTwin->name, aphexTwin->length);

      for (int i = 0; i < 4; i++) {
        enableBuzzer();
        printf("\n\nPlaying song %d st/nd/rd time.", i);
        playSong(aphexTwin);

        if (i == 2) {
          resetValues();
        }
      }

      printf("\n\nSong is over. Press button 1 to generate a new song.");
      
    } else {
       // I couldn't find info on what is the max value srand takes for % so I'll just assume the player presses the button relatively fast.
      _delay_ms(100);
      generatorSeed++;
    }
  }
  

}
