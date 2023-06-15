#define __DELAY_BACKWARD_COMPATIBLE__
#include <potentio.h>
#include <util/delay.h>
#include <stdlib.h>
#include <avr/io.h>

// Can make define for both reference voltages and all division factors.
#define REFERENCE_VOLTAGE_5_V (1 << REFS0)
#define DIVISION_FACTOR_128 ( 1 << ADPS2 ) | ( 1 << ADPS1 ) | ( 1 << ADPS0 )

void initADC5V() {
    ADMUX |= REFERENCE_VOLTAGE_5_V;    // 5V reference voltage.
    ADCSRA |= DIVISION_FACTOR_128; // Used division factor: 128
    ADCSRA |= ( 1 << ADEN ); 
}

int returnADCValue() { 
    ADCSRA |= ( 1 << ADSC );
    loop_until_bit_is_clear( ADCSRA, ADSC );
    return ADC;
}

