#include "xbox.h"

void xbox_send(Controller *c) {
    analog_write(ANALOG_0, c->joy_x);
    analog_write(ANALOG_1, -c->joy_y);
    analog_write(ANALOG_2, c->analog_l);
    analog_write(ANALOG_3, c->c_x);
    analog_write(ANALOG_4, -c->c_y);
    analog_write(ANALOG_5, c->analog_r);

    if(CONTROLLER_Y(*c)) {
        CLEAR_BIT(PORTC, PC0);
    } else {
        SET_BIT(PORTC, PC0);
    }
    if(CONTROLLER_B(*c)) {
        CLEAR_BIT(PORTC, PC1);
    } else {
        SET_BIT(PORTC, PC1);
    }
    if(CONTROLLER_A(*c)) {
        CLEAR_BIT(PORTC, PC2);
    } else {
        SET_BIT(PORTC, PC2);
    }
    if(CONTROLLER_X(*c)) {
        CLEAR_BIT(PORTC, PC3);
    } else {
        SET_BIT(PORTC, PC3);
    }
    if(CONTROLLER_L(*c) | CONTROLLER_R(*c)) {
        CLEAR_BIT(PORTC, PC4);
    } else {
        SET_BIT(PORTC, PC4);
    }
    if(CONTROLLER_START(*c)) {
        CLEAR_BIT(PORTC, PC5);
    } else {
        SET_BIT(PORTC, PC5);
    }
}
