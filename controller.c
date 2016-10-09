#include "controller.h"
#include <util/delay.h>

#define send_zeroes(count, i, n)                                \
    asm volatile(                                               \
            "ldi %[i], %[c] \n\t"                               \
            "0:ldi %[n], %[delay_zero_long] \n\t"               \
            "cbi %[port], %[bit] \n\t"                          \
            "1: dec %[n] \n\t"                                  \
            "brne 1b \n\t"                                      \
            "ldi %[n], %[delay_zero_short] \n\t"                \
            "sbi %[port], %[bit] \n\t"                          \
            "2: dec %[n] \n\t"                                  \
            "brne 2b \n\t"                                      \
            "dec %[i] \n\t"                                     \
            "brne 0b \n\t"                                      \
            "sbi %[port], %[bit] \n\t"                          \
            :                                                   \
            [i]                 "=a" (i),                       \
            [n]                 "=a" (n)                        \
            :                                                   \
            [c]                 "M" (count),                    \
            [port]              "I" (_SFR_IO_ADDR(PORT_GC)),    \
            [bit]               "I" (P_GC),                     \
            [delay_zero_long]   "M" (CYCLES_LONG / 3),          \
            [delay_zero_short]  "M" (CYCLES_SHORT / 3 - 1)      \
        )

#define send_ones(count, i, n)                                  \
    asm volatile(                                               \
            "ldi %[i], %[c] \n\t"                               \
            "0:ldi %[n], %[delay_one_short] \n\t"               \
            "cbi %[port], %[bit] \n\t"                          \
            "1: dec %[n] \n\t"                                  \
            "brne 1b \n\t"                                      \
            "ldi %[n], %[delay_one_long] \n\t"                  \
            "sbi %[port], %[bit] \n\t"                          \
            "2: dec %[n] \n\t"                                  \
            "brne 2b \n\t"                                      \
            "dec %[i] \n\t"                                     \
            "brne 0b \n\t"                                      \
            "sbi %[port], %[bit] \n\t"                          \
            :                                                   \
            [i]                 "=a" (i),                       \
            [n]                 "=a" (n)                        \
            :                                                   \
            [c]                 "M" (count),                    \
            [port]              "I" (_SFR_IO_ADDR(PORT_GC)),    \
            [bit]               "I" (P_GC),                     \
            [delay_one_long]    "M" (CYCLES_LONG / 3),          \
            [delay_one_short]   "M" (CYCLES_SHORT / 3 - 1)      \
        )

uint8_t gc_poll(uint8_t *controller_buffer) {
    SET_BIT(PORT_GC, P_GC);
    SET_BIT(DDR_GC, P_GC);          // Set PIN_GC as output

    // Temporary variables for assembly functions
    uint8_t i, n;

	// Send the initial poll message:
    //
	//     0100 0000 0000 0011 0000 0010 1
    send_zeroes(1, i, n);
    send_ones(1, i, n);
    send_zeroes(12, i, n);
    send_ones(2, i, n);
    send_zeroes(6, i, n);
    send_ones(1, i, n);
    send_zeroes(1, i, n);
    send_ones(1, i, n);

    SET_BIT(PORT_GC, P_GC);
    CLEAR_BIT(DDR_GC, P_GC);        // Set PIN_GC as input

    return 1;

    /*
    asm volatile(
            "sbic %[port], %[bit] \n\t"
            "sbi %[port], %[bit2] \n\t"
            //"rjmp 1b \n\t"
            //"0:dec %[i] \n\t"
            //"brne 0b \n\t"
            //"1: nop \n\t"
            ::
            [port]      "I" (_SFR_IO_ADDR(PORTD)),
            [bit]       "I" (PA6),
            [bit2]       "I" (PA2),
            [i]         "r" (i)
        );
    */

    /*
    // Start reading the message
    enable_usi();
    enable_timer0();
    while(1) {
        // Wait for signal to go low
        while(GET_BIT(PINA, PIN_GC)) {
            // Catch a timer overflow as an exit condition
            // This occurs if the signal is high for > 255 cycles
            if(GET_BIT(TIFR0, TOV0)) {
                // Exit condition
                disable_usi();

                // Wait for the timer to overflow and loop once, fixes 'every other' corruption
                while(TCNT0 < 24) {}

                // Check if there's no signal on the line
                if(cur_byte == 0) {
                    SET_BIT(PORTA, PIN_DEBUG);
                    CLEAR_BIT(PORTA, PIN_DEBUG);
                    return 0;
                }

                disable_timer0();
                return 1;
            }
        }
        // Reset Timer0, a little higher than 0 to account for polling delay
        TCNT0 = 5;

        // Check if a byte has passed
        if(GET_BIT(USISR, USIOIF)) {
            // Skip the counter to 8 of 16
            SET_BIT(USISR, USICNT3);    

            // Store the byte from the serial buffer
            controller_buffer[cur_byte] = USIBR;
            cur_byte++;

            // Toggle the debug pin
            //TOGGLE_BIT(PORTA, PIN_DEBUG);

            // Clear the overflow counter
            SET_BIT(USISR, USIOIF);
        }

        // Make sure signal is high before looping
        // Hardware should pull up DIN or risk an infinite loop
        while(!GET_BIT(PINA, PIN_GC)) {}
    }
    */

    // Unreachable code
    return 0;
}
