/**************************************************************************//**
 *
 * @file lock-controller.c
 *
 * @author (Rushikkumar Prajapati)
 * @author (STUDENTS --)
 *
 * @brief Code to implement the "combination lock" mode.
 *
 ******************************************************************************/

/*
 * ComboLock GroupLab assignment and starter code (c) 2022-24 Christopher A. Bohn
 * ComboLock solution (c) the above-named students
 */

#include <CowPi.h>
#include "display.h"
#include "lock-controller.h"
#include "rotary-encoder.h"
#include "servomotor.h"

static uint8_t combination[3] __attribute__((section (".uninitialized_ram.")));
volatile cowpi_ioport_t *ioport = (cowpi_ioport_t *)(0xD0000000);


uint8_t const *get_combination() {
    return combination;
}

void force_combination_reset() {
    combination[0] = 5;
    combination[1] = 10;
    combination[2] = 15;
}

void initialize_lock_controller() {
    display_string(0,"---");
    rotate_full_clockwise();
    ioport -> output  |= (1 << 21);
    ioport->output &= ~(1 << 20);

    
}

void control_lock() {
    static uint8_t entry[3] = {0xFF, 0xFF, 0xFF};
    static uint8_t index = 0;
    static char display_buffer[9];

    direction_t dir = get_direction();
    if (dir != STATIONARY) {
        if (entry[index] == 0xFF) {
            entry[index] = 0;
        }

        if (dir == CLOCKWISE) {
            entry[index] = (entry[index] + 1) % 16;
        } else if (dir == COUNTERCLOCKWISE) {
            entry[index] = (entry[index] == 0) ? 15 : entry[index] - 1;
        }
    }

    // Format display buffer: XX-XX-XX
    for (int i = 0; i < 3; i++) {
        if (entry[i] <= 15) {
            sprintf(display_buffer + i * 3, "%02d", entry[i]);
        } else {
            display_buffer[i * 3] = '-';
            display_buffer[i * 3 + 1] = '-';
        }
        if (i < 2) display_buffer[i * 3 + 2] = '-';
    }
    display_buffer[8] = '\0';
    display_string(0, display_buffer);

    // Check left button (Bit 2)
    bool left_pressed = !(ioport->input & (1 << 2));
    if (left_pressed) {
        bool match = true;
        for (int i = 0; i < 3; i++) {
            if (entry[i] != combination[i]) {
                match = false;
                break;
            }
        }

        if (match) {
            display_string(0, "OPEN");
            rotate_full_counterclockwise();
            ioport->output &= ~(1 << 21); // Left LED off
            ioport->output |= (1 << 20);  // Right LED on
        } else {
            display_string(0, "bad try 1");

            for (int i = 0; i < 2; i++) {
                ioport->output |= (1 << 21) | (1 << 20);   // LEDs on
                //sleep_ms(250);
                delay(250);
                ioport->output &= ~(1 << 21) & ~(1 << 20); // LEDs off
                //sleep_ms(250);
                delay(250);
            }

            // Reset state
            for (int i = 0; i < 3; i++) {
                entry[i] = 0xFF;
            }
            index = 0;
        }
    }


    
 }
