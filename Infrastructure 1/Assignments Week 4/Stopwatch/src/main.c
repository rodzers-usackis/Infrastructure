#define __DELAY_BACKWARD_COMPATIBLE__

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <display.h>
#include <usart.h>
#include <buttons.h>

#define MULTIPLE 250
#define FIRST_BUTTON 0
#define SECOND_BUTTON 1
#define THIRD_BUTTON 2

#define firstDigit(number) (number / 10)
#define secondDigit(number) (number % 10)

// Global variables
// Enter here your global variables
uint32_t counter;
int seconds;
int minutes;

void initTimer() {    
  // enter your code
  // Set Timer 2 to Clear Timer on Compare Match (CTC mode). As the name implies, TCNT2 will be reset when it equals OCR2A. 
  TCCR2A |= _BV(WGM21); // You do this by setting the bit WGM21 to 1 in TCCR2A.

  TIMSK2 |= _BV(OCIE2A); // Enable only the OCR2A interrupt using TIMSK2 registry.

  OCR2A = 249; // Now complete initTimer() so that this value (249) is in the OCR2A register.

  sei();
}

void startTimer() {    
    // enter your code
    TCCR2B |= _BV(CS22) | _BV(CS21); // Complete startTimer() so that Timer 2 uses a prescaler of 256. (1 << CS22) | (1 << CS21)
}

void stopTimer() {    
    // enter your code
    TCCR2B &= ~(_BV(CS22) | _BV(CS21)); // Complete stopTimer() so that Timer 2 stops counting by setting a prescaler of 0
                                                      // (0 << CS22) | (0 << CS21) | (0 << CS20) no use for last one since it was never enabled.
}

void resetTimer() {
  minutes = 0;
  seconds = 0;
  counter = 0;
}

void tick() {    
    // enter your code
  if (seconds + 1 == 60) {
    seconds = 0;
    minutes++;

  } else { // if it hits 59, I switch to 0, and don't add a second, so that 00 would be displayed briefly.
    seconds++;
  }

  printf("\n\nMinutes: %d \nSeconds %d", minutes, seconds);
}

void writeTimeAndWait(uint8_t minuten, uint8_t seconden, int delay) {    
    // enter your code
}

void displayTimer(uint8_t minutes, uint8_t seconds) {
  writeNumberToSegment(0, firstDigit(minutes));
  writeNumberToSegment(1, secondDigit(minutes));
  writeNumberToSegment(2, firstDigit(seconds));
  writeNumberToSegment(3, secondDigit(seconds));
}

// This ISR runs every 4 ms
ISR(TIMER2_COMPA_vect) {
  // Increment the counter with 1
  // If the counter + 1 is divisible by MULTIPLE, then count 1 sec
  if ((counter+1) % MULTIPLE == 0) {
    tick();
  }

  counter++;
}

int main() {    
    initUSART();    
    initDisplay();
    printf("Start the stopwatch by pressing button S1, stop by pressing button S2, and reset with S3\n");    
    initTimer();
    enableAllButtons();

    while (1) {        
        // Don't forget to initialize and start your timer
        // Also, don't forget sei()
        if (buttonPushed(FIRST_BUTTON) == 1) {
          printf("Timer started.");
          startTimer();
          _delay_ms(300); // delay so that the button wouldn't register as being pushed multiple times.
        }

        if (buttonPushed(SECOND_BUTTON) == 1) {
          printf("Timer stopped.");
          stopTimer();
          _delay_ms(300); 
        }

        if (buttonPushed(THIRD_BUTTON) == 1) {
          printf("Timer reset.");
          resetTimer();
          _delay_ms(300); 
        }

        displayTimer(minutes, seconds); // Note: Why should I make a "andWait" function if it's running in a loop.
        
    }
    return 0;
}
