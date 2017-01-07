#include "xbox.h"

void xbox_send(Controller *c) {
    // Analog values
    analog_write(ANALOG_1, c->joy_x);
    analog_write(ANALOG_2, -c->joy_y);
    analog_write(ANALOG_3, c->analog_l - 15);
    //analog_write(ANALOG_3, 0);
    analog_write(ANALOG_4, c->c_x);
    analog_write(ANALOG_5, -c->c_y);
    analog_write(ANALOG_6, c->analog_r - 15);
    //analog_write(ANALOG_6, 0);

    // A->A
    if(CONTROLLER_A(*c)) {
        CLEAR_BIT(PORTD, PD2);
        SET_BIT(DDRD, PD2);
    } else {
        CLEAR_BIT(DDRD, PD2);
        SET_BIT(PORTD, PD2);
    }
    // B->X
    if(CONTROLLER_B(*c)) {
        CLEAR_BIT(PORTD, PD0);
        SET_BIT(DDRD, PD0);
    } else {
        CLEAR_BIT(DDRD, PD0);
        SET_BIT(PORTD, PD0);
    }
    // X->B
    if(CONTROLLER_X(*c)) {
        CLEAR_BIT(PORTD, PD1);
        SET_BIT(DDRD, PD1);
    } else {
        CLEAR_BIT(DDRD, PD1);
        SET_BIT(PORTD, PD1);
    }
    // Y->Y
    if(CONTROLLER_Y(*c)) {
        CLEAR_BIT(PORTC, PC5);
        SET_BIT(DDRC, PC5);
    } else {
        CLEAR_BIT(DDRC, PC5);
        SET_BIT(PORTC, PC5);
    }

    // (Z or R)->R
    if(CONTROLLER_Z(*c) || CONTROLLER_R(*c)) {
        CLEAR_BIT(PORTC, PC3);
        SET_BIT(DDRC, PC3);
    } else {
        CLEAR_BIT(DDRC, PC3);
        SET_BIT(PORTC, PC3);
    }

    // L->L (digital)
    if(CONTROLLER_L(*c)) {
        CLEAR_BIT(PORTC, PC4);
        SET_BIT(DDRC, PC4);
    } else {
        CLEAR_BIT(DDRC, PC4);
        SET_BIT(PORTC, PC4);
    }

    // D-Up->D-Up(Taunt)
    if(CONTROLLER_D_UP(*c)) {
        CLEAR_BIT(PORTD, PD6);
        SET_BIT(DDRD, PD6);
    } else {
        CLEAR_BIT(DDRD, PD6);
        SET_BIT(PORTD, PD6);
    }
    // D-Down->View
    if(CONTROLLER_D_DOWN(*c)) {
        CLEAR_BIT(PORTC, PC0);
        SET_BIT(DDRC, PC0);
    } else {
        CLEAR_BIT(DDRC, PC0);
        SET_BIT(PORTC, PC0);
    }
    // D-Right->Xbox
    if(CONTROLLER_D_RIGHT(*c)) {
        CLEAR_BIT(PORTC, PC1);
        SET_BIT(DDRC, PC1);
    } else {
        CLEAR_BIT(DDRC, PC1);
        SET_BIT(PORTC, PC1);
    }
    // D-Left->D-Left
    if(CONTROLLER_D_LEFT(*c)) {
        CLEAR_BIT(PORTB, PB1);
        SET_BIT(DDRB, PB1);
    } else {
        CLEAR_BIT(DDRB, PB1);
        SET_BIT(PORTB, PB1);
    }

    // Start->Menu
    if(CONTROLLER_START(*c)) {
        CLEAR_BIT(PORTC, PC2);
        SET_BIT(DDRC, PC2);
    } else {
        CLEAR_BIT(DDRC, PC2);
        SET_BIT(PORTC, PC2);
    }
}
