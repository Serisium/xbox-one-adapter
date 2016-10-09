#ifndef _SPI_H
#define _SPI_H

#include <stdint.h>
#include "pins.h"

void spi_transmit(uint8_t data);
void analog_write(uint8_t channel, uint8_t data);

#endif
