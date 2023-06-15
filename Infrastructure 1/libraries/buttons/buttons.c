#define __DELAY_BACKWARD_COMPATIBLE__

#include <util/delay.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <buttons.h>

#define NUMBER_OF_BUTTONS 3

void enableButton(int button) {
    if (button < 0 || button > NUMBER_OF_BUTTONS) return;

    DDRC &= ~( 1 << (PC1 + button) );
    PORTC |= ( 1 << (PC1 + button) );
}

void enableAllButtons() {
    for (int i = 1; i <= NUMBER_OF_BUTTONS; i++) {
        DDRC &= ~( 1 << (PC1 + i) );
        PORTC |= ( 1 << (PC1 + i) );
    }
}

int buttonPushed(int button) {
    if (button < 0 || button > NUMBER_OF_BUTTONS) return 0;

    if (( PINC & ( 1 << (PC1 + button) )) == 0 ){
        return 1;
    }
    return 0;
}

int buttonReleased(int button) {
    if ( button < 0 || button > NUMBER_OF_BUTTONS ) return 0;
    
    if (( PINC & ( 1 << (PC1 + button) )) == 1 ){
        return 1;
    }
    return 0;
}

// void activateRegister() {
//     PCICR |= _BV(port);
// }

// void pinsThatActivateISR() {
//     PCMSK1 |= _BV(port);
// }