/**************************************************************************//**
 *
 * @file rotary-encoder.c
 *
 * @author (STUDENTS -- TYPE YOUR NAME HERE)
 * @author (STUDENTS -- TYPE YOUR NAME HERE)
 *
 * @brief Code to control a servomotor.
 *
 ******************************************************************************/

/*
 * ComboLock GroupLab assignment and starter code (c) 2022-24 Christopher A. Bohn
 * ComboLock solution (c) the above-named students
 */

#include <CowPi.h>
#include "servomotor.h"
#include "interrupt_support.h"

#define SERVO_PIN           (22)
#define PULSE_INCREMENT_uS  (20000)//(INT32_MAX)
#define SIGNAL_PERIOD_uS  (100)  //(INT32_MAX)

static int volatile pulse_width_us;

static void handle_timer_interrupt();

void initialize_servo() {
    cowpi_set_output_pins(1 << SERVO_PIN);
    center_servo();
    register_periodic_timer_ISR(0, PULSE_INCREMENT_uS, handle_timer_interrupt);
}

char *test_servo(char *buffer) {
    sprintf(buffer, "Pulse: %d", pulse_width_us);
    return buffer;
}

void center_servo() {
    pulse_width_us = 1500;
}

void rotate_full_clockwise() {
    pulse_width_us = 2000;
}

void rotate_full_counterclockwise() {
    pulse_width_us = 1000;
}

static void handle_timer_interrupt() {
    static int current_time_us = 0;
    static bool signal_high = false;
    volatile cowpi_ioport_t *ioport = (cowpi_ioport_t *)0xD0000000;

    if (!signal_high && current_time_us == 0) {
        ioport->output |= (1 << SERVO_PIN);  // set high
        signal_high = true;
    }

    if (signal_high && current_time_us >= pulse_width_us) {
        ioport->output &= ~(1 << SERVO_PIN); // set low
        signal_high = false;
    }

    current_time_us += PULSE_INCREMENT_uS;

    if (current_time_us >= SIGNAL_PERIOD_uS) {
        current_time_us = 0;  // start new cycle
    }
}
