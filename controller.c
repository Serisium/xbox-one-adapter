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
    controller_buffer[0] = 0b10101010;
    controller_buffer[1] = 0b01010101;

    SET_BIT(PORT_GC, P_GC);
    SET_BIT(DDR_GC, P_GC);          // Set PIN_GC as output

    // Temporary variables for assembly functions
    uint8_t i, n;

	// Send the initial poll message:
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

    //SET_BIT(PORT_DEBUG, P_DEBUG);

    // '0' bits usually take 9-10 loops
    // '1' bits usually take 3-4 loops
    asm volatile(
            //"ld %[i], %a[buff]+ \n\t"
            "ldi __tmp_reg__, 0 \n\t"
            "ldi %[i], 5 \n\t"
            "rjmp low \n\t"
            "loop: ldi %[i], 0xFF \n\t"         // Initialize timeout register
            "high: \n\t"   // Wait for the signal to go low
                "dec %[i] \n\t"                 // Decrement the timeout register
                "sbic %[pin_gc], %[bit_gc] \n\t"// Skip out if the signal goes low(bit is being transmitted)
                "brne high \n\t"                // Loop back to 0
            "breq error \n\t"                   // If we timed out instead of going low, quit
            "ldi %[i], %[threshold] \n\t"
            "low: \n\t"   // Main bit-testing loop. Takes 4n-1 clock cycles
                "dec %[i] \n\t"                 // 1c;      Decrement the timekeeping register
                "sbis %[pin_gc], %[bit_gc] \n\t"// 1/2c;    Break out once the signal goes low
                "brne low \n\t"                 // 2/1c;    Loop backwards unless we've timed out
            "breq zero \n\t"
            "one:  \n\t"
                "sbi %[port_debug], %[bit_debug] \n\t"
                "cbi %[port_debug], %[bit_debug] \n\t"
            "zero:  \n\t"
            "0: \n\t"   // Wait for the signal to go low
                "sbis %[pin_gc], %[bit_gc] \n\t"  // Skip out if the signal goes low(bit is being transmitted)
                "brne 0b \n\t"            // Loop back to 0
            "rjmp loop \n\t"
            "error: ldi %[i], 0b10101010 \n\t"
            :
            [i]                 "=a" (i),
            [n]                 "=a" (n)
            :
            [port_debug]        "I" (_SFR_IO_ADDR(PORT_DEBUG)),
            [bit_debug]         "I" (P_DEBUG),
            [port_gc]           "I" (_SFR_IO_ADDR(PORT_GC)),
            [pin_gc]            "I" (_SFR_IO_ADDR(PIN_GC)),
            [bit_gc]            "I" (P_GC),
            [buff]              "e" (controller_buffer),
            [threshold]         "M" (7)
        );

    //CLEAR_BIT(PORT_DEBUG, P_DEBUG);

    spi_transmit(i);

    return 1;
}
