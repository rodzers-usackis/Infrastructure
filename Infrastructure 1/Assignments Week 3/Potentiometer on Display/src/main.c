#include <util/delay.h>
#include <avr/io.h>
#include <usart.h>
#include <display.h>
#include <buttons.h>

void initADC() {
  // Exercise 1.
  ADMUX |= ( 1 << REFS0 );    //Set up of reference voltage. We choose 5V as reference.
  ADCSRA |= ( 1 << ADPS2 ) | ( 1 << ADPS1 ) | ( 1 << ADPS0 );  //Determine a sample rate by setting a division factor. Used division factor: 128
  ADCSRA |= ( 1 << ADEN ); //Enable the ADC

  // // Exercise 2.
  // ADMUX |= ( 1 << REFS0 ) | ( 1 << REFS1);    // Based on the Reference Selection Bits REFS0 AND REFS1 equals to Internal 2.56V Voltage Reference.
  //                                            // https://maxembedded.com/2011/06/the-adc-of-the-avr/
  //                                           // Note : Upon further inspection it looks like it was set to 1.11V maybe ?
                                              // 163 on the 5V reference equals to ~727 on the this reference.
                                           

  // ADCSRA |= ( 1 << ADPS2 ) | ( 1 << ADPS1 ) | ( 1 << ADPS0 );  //Determine a sample rate by setting a division factor. Used division factor: 128
  // ADCSRA |= ( 1 << ADEN );

  // Exercise 3. 
  // Didn't quite understand what was asked of me.
  // Was I supposed to just add (1 << PC1) to ADMUX and test it out ?
}

int main() {
  initUSART();
  initADC();
  initDisplay();

  while (1) {
    ADCSRA |= ( 1 << ADSC );    //Start the analog --> digital conversion
    loop_until_bit_is_clear( ADCSRA, ADSC );    //Wait until the conversion is completed
    uint16_t value = ADC;   //Read the result
    writeNumberAndWait(value, 1000); // writeNumber will display multiple numbers at once. this way it's perfect.

    // if (buttonPushed(0)==1) {
    //   printf("Button pushed I guess?");
    // }
  }

  return 0;
}