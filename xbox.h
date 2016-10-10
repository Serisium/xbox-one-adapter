#ifndef _XBOX_H
#define _XBOX_H

#include <stdint.h>
#include "spi.h"

void xbox_send(uint8_t *controller_buffer);

#endif
