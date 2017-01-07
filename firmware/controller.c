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
            [bit]               "I" (BIT_GC),                   \
            [delay_zero_long]   "M" (CYCLES_LONG / 3 - 0),      \
            [delay_zero_short]  "M" (CYCLES_SHORT / 3 - 2)      \
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
            [bit]               "I" (BIT_GC),                   \
            [delay_one_long]    "M" (CYCLES_LONG / 3 - 2),      \
            [delay_one_short]   "M" (CYCLES_SHORT / 3 - 0)      \
        )

uint8_t gc_poll(uint8_t *controller_buffer) {
    for(uint8_t i = 0; i < 8; i++) {
        controller_buffer[i] = 0b00000000;
    }

    SET_BIT(PORT_GC, BIT_GC);
    SET_BIT(DDR_GC, BIT_GC);          // Set PIN_GC as output

    // Temporary variables for assembly functions
    uint8_t i, n, k;

	// Send the initial poll message:
	//     0100 0000 0000 0011 0000 0010 1
    send_zeroes(1, i, n);
    send_ones(1, i, n);
    send_zeroes(12, i, n);
    send_ones(2, i, n);
    send_zeroes(8, i, n);
    //send_ones(1, i, n);
    //send_zeroes(1, i, n);
    send_ones(1, i, n);

    SET_BIT(PORT_GC, BIT_GC);
    CLEAR_BIT(DDR_GC, BIT_GC);        // Set PIN_GC as input

    // '0' bits usually take 9-10 loops
    // '1' bits usually take 3-4 loops
    // i holds the timekeeping/timeout value
    // n holds the current bit being written(0b00000001 -> 0b00000010, etc)
    // __tmp_reg__ holds the current byte, before being written to the buffer
    // k holds the number of bytes we've written, to avoid overrunning the controller_buffer
    asm volatile(
            //"ld %[i], %a[buff]+ \n\t"
            "mov __tmp_reg__, __zero_reg__ \n\t"
            "mov %[k], __zero_reg__ \n\t"
            "ldi %[n], 0b10000000 \n\t"
            "ldi %[i], 3 \n\t"
            "rjmp signal \n\t"
            "loop: ldi %[i], 0xFF \n\t"         // Initialize timeout register
            "high: \n\t"   // Wait for the signal to go low
                "dec %[i] \n\t"                 // Decrement the timeout register
                "sbic %[pin_gc], %[bit_gc] \n\t"// Skip out if the signal goes low(bit is being transmitted)
                "brne high \n\t"                // Loop back to 0
            "breq end \n\t"                     // If we timed out instead of going low, quit
            "ldi %[i], %[threshold] \n\t"
            "signal: \n\t"   // Main bit-testing loop. Takes 4n-1 clock cycles
                "dec %[i] \n\t"                 // 1c;      Decrement the timekeeping register
                "sbis %[pin_gc], %[bit_gc] \n\t"// 1/2c;    Break out once the signal goes low
                "brne signal \n\t"              // 2/1c;    Loop backwards unless we've timed out
            "breq zero \n\t"
            "one:  \n\t"
                "or __tmp_reg__, %[n] \n\t"
                "sbi %[port_debug], %[bit_debug] \n\t"
                "cbi %[port_debug], %[bit_debug] \n\t"
            "zero:  \n\t"
            "lsr %[n] \n\t" // Move on to the next bit in the series
            "brne low \n\t"     // Check if we've filled a byte
            "byte: \n\t"
                //"sbi %[port_debug], %[bit_debug] \n\t"
                "st %a[buff]+, __tmp_reg__ \n\t"        // Store the bit in the buffer
                "ldi %[n], 0b10000000 \n\t"             // Reinitialize the current bit
                "mov __tmp_reg__, __zero_reg__ \n\t"    // Zero out the byte buffer
                "inc %[k] \n\t"
                "cpi %[k], %[buffer_size] \n\t"
                "breq end \n\t"
            "low: \n\t"   // Wait for the signal to go low
                "sbis %[pin_gc], %[bit_gc] \n\t"  // Skip out if the signal goes low(bit is still being transmitted)
                "brne low \n\t"            // Loop back to 0
            "rjmp loop \n\t"
            "end: sub %[buff], %[k] \n\t"
            :
            [i]                 "=a" (i),
            [n]                 "=a" (n),
            [k]                 "=r" (k),
            [buff]              "+e" (controller_buffer)
            :
            [port_debug]        "I" (_SFR_IO_ADDR(PORT_DEBUG)),
            [bit_debug]         "I" (BIT_DEBUG),
            [port_gc]           "I" (_SFR_IO_ADDR(PORT_GC)),
            [pin_gc]            "I" (_SFR_IO_ADDR(PIN_GC)),
            [bit_gc]            "I" (BIT_GC),
            [threshold]         "M" (7),
            [buffer_size]       "M" (8)
        );

    //spi_transmit(controller_buffer[7]);

    return 1;
}

void init_calibration(Controller *controller, Calibration_Constants *calibration) {
    calibration->joy_x = controller->joy_x;
    calibration->joy_y = controller->joy_y;
    calibration->c_x = controller->c_x;
    calibration->c_y = controller->c_y;
    calibration->analog_l = controller->analog_l;
    calibration->analog_r = controller->analog_r;
}

uint8_t scale_stick(uint8_t center, uint8_t value) {
    int16_t offset = center - 127;
    int16_t adjusted_value = value + offset;
    if(adjusted_value < 0) {
        return 0;
    } else if(adjusted_value > 0xFF) {
        return 0xFF;
    } else {
        return adjusted_value;
    }
}

uint8_t scale_shoulder(uint8_t center, uint8_t value) {
    //int16_t offset = center - 80;
    if(value >= center) {
        return value - center;
    } else {
        return 0;
    }
}

void apply_calibration(Controller *controller, Calibration_Constants *calibration) {
    // Update calibration constants based on the given controller state
    controller->joy_x = scale_stick(controller->joy_x, calibration->joy_x);
    controller->joy_y = scale_stick(controller->joy_y, calibration->joy_y);
    controller->c_x = scale_stick(controller->c_x, calibration->c_x);
    controller->c_y = scale_stick(controller->c_y, calibration->c_y);
    //controller->analog_l = scale_shoulder(controller->analog_l, calibration->analog_l);
    //controller->analog_r = scale_shoulder(controller->analog_r, calibration->analog_r);

    return;
}
