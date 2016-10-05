#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define GET_BIT(TGT, PIN)    ((TGT) & (1 << (PIN))) 
#define SET_BIT(TGT, PIN)    do { TGT |=  (1 << (PIN)); } while(0)
#define CLEAR_BIT(TGT, PIN)  do { TGT &= ~(1 << (PIN)); } while(0)
#define TOGGLE_BIT(TGT, PIN) do { TGT ^=  (1 << (PIN)); } while(0)

#define LEFT_WAIT   4
#define RIGHT_WAIT  4

typedef enum {
    Idle_left,
    Idle_right,
    Move_left,
    Move_right
} State;

int state = 0;
int count = 0;

void spi_transmit(uint8_t data) {
    SET_BIT(SPCR, MSTR);
    SPDR = data;

    while(!GET_BIT(SPSR, SPIF));
}

void analog_write(uint8_t channel, uint8_t data) {
    CLEAR_BIT(PORTB, PB2);
    spi_transmit(0x0f & channel);
    spi_transmit(data);
    SET_BIT(PORTB, PB2);
}


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

    // Set up SPI for analog outputs, ch 19.5 in datasheet
    // Enable latch, MOSI, and SCK as outputs
    SET_BIT(DDRB, PB2);
    SET_BIT(DDRB, PB3);
    SET_BIT(DDRB, PB5);
    // Set latch as idle-high
    SET_BIT(PORTB, PB2);
    // Enable SPI in Master mode, with a rate of F_CPU/64
    SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR1);

    uint8_t analog = 0;

    while(1) {
        wdt_reset();

        analog++;
        analog_write(8, analog);

        if(state == Move_left) {
            DDRC = 0b00000001;
            PORTC = 0b11111110;
            state = 1;
        } else if(state == Move_right) {
            DDRC = 0b00000010;
            PORTC = 0b11111101;
            state = 0;
        }
        _delay_us(500);
    }
}

// Watch the USB D- line for activity, so we can stay synchronized
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
        SET_BIT(PORTD, PD3);
        _delay_us(20);
        CLEAR_BIT(PORTD, PD3);
        count++;
        if((count >= LEFT_WAIT) & (state == Idle_left)) {
            state = Move_left;
            count = 0;
        } else if((count >= RIGHT_WAIT) & (state == Idle_right)) {
            state = Move_right;
            count = 0;
        }
    }
}
