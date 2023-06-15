#include <util/delay.h>
#include <avr/io.h>
#include <usart.h>

/* In this code we'll use some predefined macros, like _BV and bit_is_clear
 * These macros make the code more readable.
 * They are defined in avr/sfr_defs.h and are included in avr/io.h. */

int main()
{
    initUSART();
    DDRC &= ~_BV( PC1 );    /* Set up DDR, instead of using ( 1 << PC1 ) we use the
                             * macro _BV( bit ). (BV stands for "Bit Value") */
    printf( "DDRC: " );
    printBinaryByte( DDRC );
    PORTC |= _BV( PC1 );    /* Activate pull-up of C1.
                             * We use the macro _BV( bit ) instead of ( 1 << PC 1 ) */
    printf( "\nPORTC: " );
    printBinaryByte( PORTC );
    while ( 1 )
    {
        printf( "\nPINC: " );
        printBinaryByte( PINC );
        if ( bit_is_clear( PINC, PC1 )) /* instead of (!(PINC & ( 1 << PC1 ))) we use the macro bit_is_clear */
        {
            printf( " - Button 1 pressed!\n" );
        }
        else
        {
            printf( " - Button 1 NOT pressed!!\n" );
        }
        _delay_ms( 1000 );
    }
    return 0;
}