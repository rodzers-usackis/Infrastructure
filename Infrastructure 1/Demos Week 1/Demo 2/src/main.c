#include <util/delay.h>
#include <avr/io.h>
#include <flashing_leds.h>

int main()
{
    for ( int i = 0; i < 4; i++ )
    {
        enableLed(i);
    }
    while (1)
    {
        for ( int i = 0; i < 4; i++ )
        {
            lightUpLed(i);
            _delay_ms( 300 );
        }
        for ( int i = 0; i < 4; i++ )
        {
            lightDownLed(i);
            _delay_ms( 300 );
        }
    }
    return 0;
}

