#ifndef _SPI_H
#define _SPI_H

#include <stdint.h>
#include "pins.h"

// Define channel IDs

#define ANALOG_0    0b00001000
#define ANALOG_1    0b00000100
#define ANALOG_2    0b00001100
#define ANALOG_3    0b00000010
#define ANALOG_4    0b00001010
#define ANALOG_5    0b00000110

void spi_transmit(uint8_t data);
void analog_write(uint8_t channel, uint8_t data);

#endif
