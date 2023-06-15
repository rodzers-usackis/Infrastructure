#include <util/delay.h>
#include <avr/io.h>
#include <flashing_leds.h>

int main(){ 
 
 // Test with everything that was made in 1.11 and 1.12.
 uint8_t allLeds = 0b00111100; 
 
 enableLed(3); // enabling the leds I'll be using.

 lightUpLed(3);
 _delay_ms(2000);

 dimLed(3, 10, 2000);
 _delay_ms(1000);

 fadeInLed(3, 1000);
 _delay_ms(1000);

 lightDownLed(3);
_delay_ms(1000);

 enableLeds(allLeds);
 lightUpLeds(allLeds);
 _delay_ms(1000);
 lightDownLeds(allLeds);

   
return 0;

}