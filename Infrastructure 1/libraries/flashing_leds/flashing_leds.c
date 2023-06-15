#define __DELAY_BACKWARD_COMPATIBLE__

#include <util/delay.h>
#include <avr/io.h>
#include <flashing_leds.h>
#include <stdio.h>
#include <stdlib.h>


#define NUMBER_OF_LEDS 4 //Define is a "preprocessor directive". It ensures that every NUMBER_OF_LEDS will be replaced by 4 in the following code
#define ALL_LEDS 0b00111100

void enableLed ( int lednumber ) //C has no classes; functions can be included directly in the .c file.
{
    if ( lednumber < 0 || lednumber > NUMBER_OF_LEDS-1 ) return;
    DDRB |= ( 1 << ( PB2 + lednumber ));    //Check the tutorial "Writing to a Pin". We know from the documentation on
                                            //the multifunctional shield that the LEDs start at PB2
}

void lightUpLed ( int lednumber )    //Note: enabled LEDs light up immediately ( 0 = on )
{
    if ( lednumber < 0 || lednumber > NUMBER_OF_LEDS-1 ) return;
    PORTB &= ~( 1 << ( PB2 + lednumber ));  //Check the tutorial on "Bit Operations" to know what happens in this line.
}

void lightDownLed ( int lednumber )
{
    if ( lednumber < 0 || lednumber > 3 ) return;
    PORTB |= ( 1 << ( PB2 + lednumber ));   //Make sure you understand this line as well!
}

// 1.11 
// Did it the cheap way.
// IF SOMETHING DOESN'T WORK, RETURN TO HERE.

void enableLeds (uint8_t leds) { // Since I'm already selecting which leds i'll be using, I'm not adding the ability to enable extra individual leds later.
    DDRB |= leds;
}

void lightUpLeds (uint8_t leds) { // If I pass first two leds, later last two leds - all four will be lit.
    PORTB &= ~(leds);         
}

void lightDownLeds (uint8_t leds) { 
    PORTB |= leds;
}

void enableAllLeds () { // Created these while working on Nim project, that's why they're not used prior to that.
    DDRB |= ALL_LEDS;
}

void lightUpAllLeds () { 
    PORTB &= ~(ALL_LEDS);         
}

void lightDownAllLeds () { 
    PORTB |= ALL_LEDS;
}

// 1.12 
// Judging by the task, I'm assuming the LED has already been enabled.
// I'm also reusing the previously created functions, because I don't think there's a reason to type them out all over again.
void dimLed (int lednumber, int percentage, int duration) {
    if ( lednumber < 0 || lednumber > NUMBER_OF_LEDS-1 ) return;
    if (percentage < 0 || percentage > 100) return;

    double lightOff = (100-percentage)/10;
    double lightOn = 10-lightOff;

  
        for(int i = 0; i<duration/10; i++) { // The delays are always 10ms, I'm assuming that the program runs instantaniously.
                                            // Fix later with real timers*
            lightUpLed(lednumber);
            _delay_ms(lightOn);
            lightDownLed(lednumber);
            _delay_ms(lightOff);
        }

}

void dimLedsWithoutDuration (uint8_t leds, int percentage) {
    if (percentage < 0 || percentage > 100) return;

    double lightOff = (100-percentage)/10;
    double lightOn = 10-lightOff;
    
    while(1) {
        lightUpLeds(leds);
        _delay_ms(lightOn);
        lightDownLeds(leds);
        _delay_ms(lightOff);
    }
}

void fadeInLed (int lednumber, int duration) { // NOTE: THIS FUNCTION ONLY WORKS IF THE DURATION IS 1000ms OR ABOVE.
    if ( lednumber < 0 || lednumber > NUMBER_OF_LEDS-1 ) return;

// FadeOut
    // for (int i = 100; i>=0; i--) {
    //     dimLed(lednumber, i, (duration/100));
    // }

// FadeIn
    for (int i = 0; i<=100; i++) {
        dimLed(lednumber, i, (duration/100));
    }
}

void flashingLed (int lednumber, int numberOfFlashes, int delay) {
    if ( lednumber < 0 || lednumber > NUMBER_OF_LEDS-1 ) return;

    for (int i = 0; i<numberOfFlashes; i++) {
        ledOnAndOffWithDelay(lednumber, delay);
    }
}

void flashingLeds (uint8_t leds, int numberOfFlashes, int delay) {
    for (int i = 0; i<numberOfFlashes; i++) {
        ledsOnAndOffWithDelay(leds, delay);
    }
}

void ledOnAndOffWithDelay (int lednumber, int delay) { // Created to remove copy & pasted text.
    lightUpLed(lednumber);
    _delay_ms(delay);
    lightDownLed(lednumber);
    _delay_ms(delay);
}

void ledsOnAndOffWithDelay (uint8_t leds, int delay) { // Created to remove copy & pasted text.
    lightUpLeds(leds);
    _delay_ms(delay);
    lightDownLeds(leds);
    _delay_ms(delay);
}

// For Week 4 Project
void flashingLedsForDuration(uint8_t leds, int delay, int duration)  { // bad name I know, but recognizable. for me at least...
    for (int i = 0; i < duration / delay; i++) {
        lightUpLeds(leds);
        _delay_ms(delay/2);
        lightDownLeds(leds);
        _delay_ms(delay/2);
    }
}

// For Week 1 Project

void knightriderLedTheme() {

    uint8_t allLeds = 0b00111100; 

    enableLeds(allLeds); // just in case all Leds aren't enabled.
    lightDownLeds(allLeds);
    
    while(1) {

        for (int i = 0; i < NUMBER_OF_LEDS-1; i++) {
            lightUpLed(i);
            _delay_ms(150);
            lightDownLed(i);
        }

        for (int i = NUMBER_OF_LEDS-1; i > 0; i--) {
            lightUpLed(i);
            _delay_ms(150);
            lightDownLed(i);
        }
    }
}