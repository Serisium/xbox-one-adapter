#ifndef _CONTROLLER_H
#define _CONTROLLER_H

#include <stdint.h>
#include "pins.h"
#include "spi.h"

#define DEADZONE_L 80
#define DEADZONE_R 80
#define DEADZONE_X 60
#define DEADZONE_Y 60
#define CENTER_X 127
#define CENTER_Y 127

#pragma pack(push, 0)
typedef struct Controller_t {
    //uint8_t console_message[3];
    uint16_t button_states;
    uint8_t joy_x;
    uint8_t joy_y;
    uint8_t c_x;
    uint8_t c_y;
    uint8_t analog_l;
    uint8_t analog_r;
} Controller;
#pragma pack(pop)

#pragma pack(push, 0)
typedef struct Keyboard_t {
    //uint8_t console_message[3];
    uint32_t header;
    uint8_t button[3];
    uint8_t footer;
} Keyboard;
#pragma pack(pop)

typedef enum Button_t {
    Button_a = 0, Button_b, Button_x, Button_y, Button_start,
    Button_d_left = 8, Button_d_right, Button_d_down, Button_d_up, Button_z, Button_r, Button_l
} Button;

#define CONTROLLER_BUTTON(C, B) (((C).button_states & (1 << (B))) != 0)

#define CONTROLLER_START(C) (CONTROLLER_BUTTON((C), Button_start))
#define CONTROLLER_Y(C) (CONTROLLER_BUTTON((C), Button_y))
#define CONTROLLER_X(C) (CONTROLLER_BUTTON((C), Button_x))
#define CONTROLLER_B(C) (CONTROLLER_BUTTON((C), Button_b))
#define CONTROLLER_A(C) (CONTROLLER_BUTTON((C), Button_a))
#define CONTROLLER_L(C) (CONTROLLER_BUTTON((C), Button_l))
#define CONTROLLER_R(C) (CONTROLLER_BUTTON((C), Button_r))
#define CONTROLLER_Z(C) (CONTROLLER_BUTTON((C), Button_z))
#define CONTROLLER_D_UP(C) (CONTROLLER_BUTTON((C), Button_d_up))
#define CONTROLLER_D_DOWN(C) (CONTROLLER_BUTTON((C), Button_d_down))
#define CONTROLLER_D_RIGHT(C) (CONTROLLER_BUTTON((C), Button_d_right))
#define CONTROLLER_D_LEFT(C) (CONTROLLER_BUTTON((C), Button_d_left))

#define ANALOG_L(C) ((C).analog_l > DEADZONE_L)
#define ANALOG_R(C) ((C).analog_r > DEADZONE_R)

#define ANALOG_UP(C) ((C).joy_y > CENTER_Y + DEADZONE_Y)
#define ANALOG_DOWN(C) ((C).joy_y < CENTER_Y - DEADZONE_Y)
#define ANALOG_LEFT(C) ((C).joy_x < CENTER_X - DEADZONE_X)
#define ANALOG_RIGHT(C) ((C).joy_x > CENTER_X + DEADZONE_X)

#define C_UP(C) ((C).c_y > CENTER_Y + DEADZONE_Y)
#define C_DOWN(C) ((C).c_y < CENTER_Y - DEADZONE_Y)
#define C_LEFT(C) ((C).c_x < CENTER_X - DEADZONE_X)
#define C_RIGHT(C) ((C).c_x > CENTER_X + DEADZONE_X)

#define CYCLES_LONG         3 * F_CPU / 1000000
#define CYCLES_SHORT        1 * F_CPU / 1000000

typedef struct Calibration_Constants_t {
    uint8_t joy_x, joy_y,   
            c_x, c_y,
            analog_l, analog_r;
} Calibration_Constants;

uint8_t gc_poll(uint8_t *controller_buffer);
void init_calibration(Controller *controller, Calibration_Constants *cal);
void apply_calibration(Controller *controller, Calibration_Constants *cal);

#endif
