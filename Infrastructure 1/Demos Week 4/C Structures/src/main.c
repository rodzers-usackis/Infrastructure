#include <avr/io.h>
#include <stdlib.h>
#include <usart.h>
#include <util/delay.h>
#include <string.h>

//We define our own datatype: CARD with 2 fields: value and suit
typedef struct
{
   int value;
   char suit[10]; 
} CARD;

// This function receives the array of cards as a parameter
// and fills this with 4 * 13 playing cards.

void fullDeck( CARD deck[52] )
{
    char suits[4][10] = { "hearts","diamonds","spades","clubs" };
    int index;

    for ( int i = 0; i < 4; i++ )
    {
        for ( int j = 0; j < 13; j++ )
        {
            index = ( i * 13 ) + j;
            deck[index].value = ( j + 1 );
            strcpy( deck[index].suit, suits[i] );
        }
    }
}

// This function shows te CARD
void showCard( CARD theCard )
{
    switch ( theCard.value )
    {
        // special cards:
        case 1:
            printf( "ace of %s", theCard.suit );
            break;
        case 11:
            printf( "jack of %s", theCard.suit );
            break;
        case 12:
            printf( "queen of %s", theCard.suit );
            break;
        case 13:
            printf( "king of %s", theCard.suit );
            break;
        default:    //all other cards:
            printf( "%d of %s", theCard.value, theCard.suit );
    }
}

// This function draws a random card from the array
CARD drawCard( CARD deck[] )
{
    //The random generator is NOT seeded, so the cards are the same on every run
    int randm = rand() % 52;
    return deck[randm];
}

int main()
{
    initUSART();
    CARD deck[52];  // an array of structures
    fullDeck( deck );
    // Draw 10 random cards and show them:
    for ( int i = 0; i < 10; i++ )
    {
        CARD card = drawCard( deck );
        showCard( card );
        printf( "\n" );
    }
    return 0;
}