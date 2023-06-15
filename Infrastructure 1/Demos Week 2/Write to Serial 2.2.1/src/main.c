#include <util/delay.h>
#include <avr/io.h>
#include <usart.h>  /* This preprocessor directive makes sure that all the function declarations
                     * of the usart library are loaded.
                     * Check the tutorial of week 1: "1.8 Using your own library in VS Code" */

int main()
{
    DDRB |= ( 1 << PB2 );
    initUSART();    /* We call a function from usart.h.
                     * This function initializes the communication with the serial port. */
    int counter = 0;
    while (1)
    {
        PORTB = ( 1 << PB2 );
        _delay_ms( 100 );
        PORTB = ( 0 << PB2 );
        _delay_ms( 100 );
        printString( " Debugging!! " );  /* We call a function from usart.h.
                                         * If everything is ok, then this text
                                         * should appear at the bottom of the screen,
                                         * in the Serial Monitor. */
        printf( "Counter: %d\n", counter ); /* We call the printf function, from the standard C library.
                                             * This function is very similar to System.out.printf in Java. */
        counter++;
    }
    return 0;
}