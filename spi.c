#include "spi.h"

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
