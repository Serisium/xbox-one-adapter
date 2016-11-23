#ifndef _PINS_H
#define _PINS_H

#include <avr/io.h>

#define GET_BIT(TGT, PIN)    ((TGT) & (1 << (PIN))) 
#define SET_BIT(TGT, PIN)    do { TGT |=  (1 << (PIN)); } while(0)
#define CLEAR_BIT(TGT, PIN)  do { TGT &= ~(1 << (PIN)); } while(0)
#define TOGGLE_BIT(TGT, PIN) do { TGT ^=  (1 << (PIN)); } while(0)

#define PORT_DEBUG  PORTD
#define PIN_DEBUG   PIND
#define DDR_DEBUG   DDRD
#define BIT_DEBUG   PD3

#define PORT_GC     PORTD
#define PIN_GC      PIND
#define DDR_GC      DDRD
#define BIT_GC      PD5

#endif
