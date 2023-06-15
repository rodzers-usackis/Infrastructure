#define __DELAY_BACKWARD_COMPATIBLE__

#include <avr/io.h>
#include <stdbool.h>
#include <stdlib.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <usart.h>


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

bool playing = false;
int frequency;

ISR(PCINT1_vect) { 
  if (buttonPushed(FIRST_BUTTON) || buttonPushed(SECOND_BUTTON) || buttonPushed(THIRD_BUTTON)) { // If any button is pressed, playing turns to the opposite value.
    _delay_us(1000);
    if (buttonPushed(FIRST_BUTTON) || buttonPushed(SECOND_BUTTON) || buttonPushed(THIRD_BUTTON)) {
      playing = !playing;  
      playing ? enableBuzzer() : disableBuzzer(); // Created this function cause otherwise the buzzer buzzed at the beggining just from enableBuzzer()
      printf(playing ? "\nPlaying a %d hertz note.\n" : "\nNot playing a sound. Silent.\n", frequency);
    }
  }
}

int main() {
  initUSART();
  initADC5V();
  initDisplay();
  enableAllButtons();

  PCICR |= _BV(PCIE1);
  PCMSK1 |= _BV(BUTTON1) | _BV(BUTTON2) | _BV(BUTTON3);
  sei();

 

  while (1) {
    // Make sure that the user can choose a frequency by turning the screw of the potentiometer. The frequency is shown on the display.
    if(playing == false) {
      // frequency = returnADCValue()*20; // Based on the frequency range that the human ear is able to understand
                                      // which is between 20 Hz and 20.000Hz or 20 kHz (1kHz = 1 Kilohertz or 1000 Hz).

      frequency = returnADCValue(); // Nicer to the ears :) Edit: nevermind, still painful
      writeNumberAndWait(frequency, 250); 
    // As soon as you press a button, the tone will be played.
    // If you press the button again, the tone will stop. ( With the help of ISR() turning the playing value opposite )
    } else {
      playTone(frequency, 500);
    }
  }

  return 0;
}