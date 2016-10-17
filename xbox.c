#include "xbox.h"

void xbox_send(Controller *c) {
    analog_write(ANALOG_0, c->joy_x);
    analog_write(ANALOG_1, -c->joy_y);
    analog_write(ANALOG_2, c->analog_l);
    analog_write(ANALOG_3, c->c_x);
    analog_write(ANALOG_4, -c->c_y);
    analog_write(ANALOG_5, c->analog_r);
}
