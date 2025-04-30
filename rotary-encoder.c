/**************************************************************************//**
 *
 * @file rotary-encoder.c
 *
 * @author (Rushikkumar Prajapati)
 * @author (STUDENTS -- TYPE YOUR NAME HERE)
 *
 * @brief Code to determine the direction that a rotary encoder is turning.
 *
 ******************************************************************************/

/*
 * ComboLock GroupLab assignment and starter code (c) 2022-24 Christopher A. Bohn
 * ComboLock solution (c) the above-named students
 */

#include <CowPi.h>
#include "interrupt_support.h"
#include "rotary-encoder.h"

#define A_WIPER_PIN         (16)
#define B_WIPER_PIN         (A_WIPER_PIN + 1)

typedef enum {
    HIGH_HIGH, HIGH_LOW, LOW_LOW, LOW_HIGH, UNKNOWN
} rotation_state_t;

static rotation_state_t volatile state;
static direction_t volatile direction = STATIONARY;
static int volatile clockwise_count = 0;
static int volatile counterclockwise_count = 0;

static void handle_quadrature_interrupt();

void initialize_rotary_encoder() {
    cowpi_set_pullup_input_pins((1 << A_WIPER_PIN) | (1 << B_WIPER_PIN));
    
   register_pin_ISR((1 << A_WIPER_PIN) | (1 << B_WIPER_PIN), handle_quadrature_interrupt);
}

uint8_t get_quadrature() {
   /////////////////////////////////////////////////////////
   volatile cowpi_ioport_t *ioport = (cowpi_ioport_t *)(0xD0000000);
   uint32_t a = ioport->input & (1 << 16);


    uint32_t b =(ioport->input & (1 << 17));// (B_WIPER_PIN);
    return ( a + b)  >> 16    ; 
    
    //bool pressed = !(ioport->input & (1 << 3));
}

char *count_rotations(char *buffer) {
    sprintf(buffer, "CW:%d CCW:%d", clockwise_count, counterclockwise_count);
    return buffer;
}

direction_t get_direction() {
    direction_t value =  direction;
    direction = STATIONARY;


    
    return value;
}

static void handle_quadrature_interrupt() {
    static rotation_state_t last_state = UNKNOWN;
    static rotation_state_t before_state = UNKNOWN;
    uint8_t quadrature = get_quadrature();
    
    switch (quadrature){
        case 0b11:
        state = HIGH_HIGH;
        break;
        case 0b10:
            state = HIGH_LOW;
            break;
        case 0b00:
            state = LOW_LOW;
            break;
        case 0b01:
            state = LOW_HIGH;
            break;
        default:
            state = UNKNOWN;
            break;
    }

    // Determine rotation
    if (last_state == HIGH_HIGH) {
        if (state == HIGH_LOW) {
            //direction = CLOCKWISE;
            
        } else if (state == LOW_HIGH) {
            //direction = COUNTERCLOCKWISE;
            
        }
    } else if (last_state == HIGH_LOW) {
        if (state == LOW_LOW && before_state == HIGH_HIGH ) {
            direction = CLOCKWISE;

            clockwise_count++;

        } else if (state == HIGH_HIGH) {
            
                    }
    } else if (last_state == LOW_LOW) {
        if (state == LOW_HIGH) {
            
        } else if (state == HIGH_LOW) {
           
        }
    } else if (last_state == LOW_HIGH) {
        if (state == HIGH_HIGH) {
           
        } else if (state == LOW_LOW && before_state == HIGH_HIGH ) {
            direction = COUNTERCLOCKWISE;
            counterclockwise_count++;
        }
    }

    
    before_state = last_state;
    last_state = state;

}

