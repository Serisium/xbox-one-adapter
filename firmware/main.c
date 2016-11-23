#include <stdint.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "xbox.h"
#include "pins.h"
#include "controller.h"
#include "spi.h"

int main(void)
{
    //wdt_enable(WDTO_1S);    // enable 1s watchdog timer

    // Enable external interrupt on INT0(PD2) rising edge
    //EICRA = (1 << ISC01) | (1 << ISC00);    // INT0 Rising edge
    //EIMSK = (1 << INT0);                    // Enable INT0

    // Enable interrupts
    //sei();
    cli();

    // Set BIT_DEBUG as debug output line
    SET_BIT(DDR_DEBUG, BIT_DEBUG);

    // Set pins high
    PORTB = 0xFF;
    PORTC = 0xFF;
    PORTD = 0xFF;

    // Set XBox interface pins as output
    DDRB = (1<<PB0) || (1<<PB1);
    DDRC = (1<<PC0) || (1<<PC1) || (1<<PC2) || (1<<PC3) || (1<<PC4) || (1<<PC5);
    DDRD = (1<<PD0) || (1<<PD1) || (1<<PD2) || (1<<PD3) || (1<<PD4) || (1<<PD6) || (1<<PD7);

    // Set up SPI for analog outputs, ch 19.5 in datasheet
    // Enable SS, MOSI, and SCK as outputs
    SET_BIT(DDRB, PB2); // SS
    SET_BIT(DDRB, PB3); // MOSI
    SET_BIT(DDRB, PB5); // SCK
    // Set latch as idle-high
    SET_BIT(PORTB, PB2);
    // Enable SPI in Master mode, with a rate of F_CPU/64
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1);

    uint8_t controller_buffer[8] = {0};
    Controller *controller = (Controller*)controller_buffer;
    Cal calibration_constants[6] = {0};
    init_calibration(calibration_constants);

    while(1) {
        wdt_reset();

        for(uint8_t i = 0; i < 8; ++i) {
            controller_buffer[i] = 0x00;
        }

		gc_poll(controller_buffer);
        apply_calibration(controller, calibration_constants);
        xbox_send(controller);

        _delay_us(4000);
    }
}

// Watch the USB D- line for activity, so we can stay synchronized
/*
ISR(INT0_vect) {
    // Wait out a sync pulse's duration
    _delay_us(15);

    // Clear the interrupt flag, so we won't double-trigger on previous pulses
    SET_BIT(EIFR, INTF0);

    // Wait out another 2us, to give INTF0 a chance to trigger
    _delay_us(2);

    // See if there's more activity on D-
    // If so, this is a data transfer. If not, it was just a sync pulse
    if(GET_BIT(EIFR, INTF0)) {
        // Toggle the debug line
        SET_BIT(PORT_DEBUG, BIT_DEBUG);
        _delay_us(20);
        CLEAR_BIT(PORT_DEBUG, BIT_DEBUG);
    }
}
*/
