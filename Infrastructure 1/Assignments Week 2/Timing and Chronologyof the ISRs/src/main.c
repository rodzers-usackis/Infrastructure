#include <avr/io.h>
#include <flashing_leds.h>
#include <buttons.h>
#include <util/delay.h>
#include <usart.h> // not used
#include <avr/interrupt.h>

#define __DELAY_BACKWARD_COMPATIBLE__  
#define BUTTON_1 0
#define BUTTON_2 1
#define S1 PC1
#define S2 PC2
#define ALL_LEDS 0b00111100


/* Questions (1) : 
Does LED D1 continue to flash when the ISR has started?
    No, it stays in the state it was in when the ISR started.

Does LED D1 and D2 continue to flash after the ISR has been handled (ie is it over)?
    Yes.

If LED D1 and D2 were on before starting the ISR, are they still on after running the ISR? 
    The D1 and D2 loop is paused until the ISR is done, and since the ISR turns off D2 led, only D1 is lit up (until the loop restarts again).

If LED D1 and D2 were off before starting the ISR, are they still off after running the ISR?
    Yes, as mentioned previously, their state remains paused until the ISR ends.

If LEDs D1 and D2 were on for only 0.5 seconds before starting the ISR, will they keep their status for 1.5 seconds after performing the ISR?
    No, only D1 will keep it's status, because the ISR turned D2 off.

   Questions (2) :
Activate the functionality associated with button S2 after starting the program.

How does this affect the execution of the program?
    The same as the code associated with button S1 ISR(), the original code loop is paused until the ISR finishes.

Activate the functionality associated with button S2 during the execution of the ISR associated with pressing button S1. How does this affect the execution of the program / the ISR?
    Functionality associated with button S2 doesn't work, because the button S1 ISR() is in full effect (You can only ISR() S2 when the S1 ISR() finishes).

*/
ISR( PCINT1_vect )
{
    // button 1 is pressed (bit is set to 0)?
    if (buttonPushed(BUTTON_1) == 1)
    {
        //We wait 1000 microseconds and check again (debounce!)
        _delay_us( 1000 );
        // button 0 is pressed (bit is set to 0)?
        if (buttonPushed(BUTTON_1) == 1)
        {
            printf("Button 1 has been pressed.\n");
            ledsOnAndOffWithDelay(0b00011000, 5000);
        }
    }

    if (buttonPushed(BUTTON_2) == 1)
    {
        //We wait 1000 microseconds and check again (debounce!)
        _delay_us( 1000 );
        // button 0 is pressed (bit is set to 0)?
        if (buttonPushed(BUTTON_2) == 1)
        {
            printf("Button 2 has been pressed.\n");
            flashingLed(2, 7, 500);
        }
    }
}


int main() {
  initUSART();

  enableButton(BUTTON_1);
  PCICR |= _BV(PCIE1);
  PCMSK1 |= _BV(S1);
  PCMSK1 |= _BV(S2);


  enableLeds(ALL_LEDS);
  lightDownLeds(ALL_LEDS);
  
  _delay_ms(1000);

  sei();
  // When starting this program, LEDs D1 and D2 start flashing together on and off with an interval of 4 seconds (2 seconds together on, 2 seconds together off).
  while(1) {
    ledsOnAndOffWithDelay(0b00110000, 2000); // Could also make a constant value or #define for the leds value, but didn't this time.
  }
  cli();

  return 0;
}