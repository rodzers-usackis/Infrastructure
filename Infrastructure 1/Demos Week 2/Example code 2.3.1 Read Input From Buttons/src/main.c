#include <util/delay.h>
#include <avr/io.h>
#include <usart.h>

int main()
{
    initUSART();
    DDRC &= ~( 1 << PC1 );  /* Button 1 is connected to C1.
                             * We set the corresponding bit in the Data Direction Register to 0. */
    printf( "DDRC: " );
    printBinaryByte( DDRC );
    PORTC |= ( 1 << PC1 );  /* Activate the Pull-up of C1: PINC will be set to high */
    printf( "\nPORTC: " );
    printBinaryByte( PORTC );
    while ( 1 )
    {
        printf( "\nPINC: " );
        printBinaryByte( PINC );
        if (( PINC & ( 1 << PC1 )) == 0 )   /* This way we can test whether the PC1 bit
                                             * of PINC is set to 0 (button is pressed) */
        {
            printf( " - Button 0 pressed!\n" );
        }
        else
        {
            printf( " - Button 0 NOT pressed!!\n" );
        }
        _delay_ms( 1000 );
    }
    return 0;
}