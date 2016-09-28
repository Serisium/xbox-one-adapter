#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define GET_BIT(TGT, PIN)    ((TGT) & (1 << (PIN))) 
#define SET_BIT(TGT, PIN)    do { TGT |=  (1 << (PIN)); } while(0)
#define CLEAR_BIT(TGT, PIN)  do { TGT &= ~(1 << (PIN)); } while(0)
#define TOGGLE_BIT(TGT, PIN) do { TGT ^=  (1 << (PIN)); } while(0)

int main(void)
{
    wdt_enable(WDTO_1S);    // enable 1s watchdog timer

    // Enable external interrupt on INT0(PD2) rising edge
    EICRA = (1 << ISC01) | (1 << ISC00);    // INT0 Rising edge
    EIMSK = (1 << INT0);                    // Enable INT0

    // Enable interrupts
    sei();

    // Set PD3 as debug output line
    SET_BIT(DDRD, PD3);

    // Set PORTC high
    PORTC = 0xFF;

    // Set PORTC as output
    DDRC = 0xFF;


    while(1) {
        wdt_reset();

        DDRC = 0b00000001;
        PORTC = 0b000000010;
        _delay_us(16500);

        DDRC = 0b00000010;
        PORTC = 0b000000001;
        _delay_us(16500);
    }
}

// Watch the USB D- line for activity, so we can stay synchronized
ISR(INT0_vect) {
    // Wait out a sync pulse's duration
    _delay_us(1);

    // Clear the interrupt flag, so we won't double-trigger on previous pulses
    SET_BIT(EIFR, INTF0);

    // Wait out another 2us
    _delay_us(2);

    // See if there's more activity on D-
    // If so, this is a data transfer. If not, it was just a sync pulse
    if(GET_BIT(EIFR, INTF0)) {
        // Toggle the debug line
        SET_BIT(PORTD, PD3);
        _delay_us(5);
        CLEAR_BIT(PORTD, PD3);
    }
}
