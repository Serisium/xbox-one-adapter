#ifndef _XBOX_H
#define _XBOX_H

#include <stdint.h>
#include "spi.h"
#include "controller.h"

void xbox_send(Controller *c);

#endif
