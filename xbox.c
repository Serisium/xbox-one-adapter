#include "xbox.h"

void xbox_send(uint8_t *controller_buffer) {
    analog_write(ANALOG_0, controller_buffer[2]);
    analog_write(ANALOG_1, controller_buffer[3]);
    analog_write(ANALOG_2, controller_buffer[6]);
    analog_write(ANALOG_3, controller_buffer[4]);
    analog_write(ANALOG_4, controller_buffer[5]);
    analog_write(ANALOG_5, controller_buffer[7]);
}
