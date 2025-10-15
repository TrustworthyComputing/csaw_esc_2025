#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "../simpleserial/simpleserial.h"

#define SEQUENCE_LENGTH 3

int secret_ignition_sequence[SEQUENCE_LENGTH]; //REDACTED
int calibration_sequence[SEQUENCE_LENGTH];
uint32_t queries = 0 ;

static const uint8_t flag[17]; //REDACTED


void initialize_sequences(void) {
    for (uint8_t i = 0; i < SEQUENCE_LENGTH; i++) {
        calibration_sequence[i] = secret_ignition_sequence[i];
    }
}

uint8_t arm_system(uint8_t* data, uint8_t dlen) {
    uint8_t success = 1;
    uint8_t failure = 0;
    uint8_t match_count = 0 ;
    const char false_flag[] = "FailureToLaunch!\n" ; 
    const char flag[18] ;
    queries ++ ;
    
    if (dlen != sizeof(secret_ignition_sequence)) {
        simpleserial_put('r', 17, (uint8_t*) false_flag);
        return 0x00;
    }

    transform_data(dak, 0, nonce, dflag, flag, 17);
    int* user_sequence = (int*)data;

    for (int i = 0; i < SEQUENCE_LENGTH; i++) {
        if (secret_ignition_sequence[i] == user_sequence[i]) 
	{
            match_count ++ ;
        }
    }

    if (match_count == SEQUENCE_LENGTH) {
        simpleserial_put('r', 17, (uint8_t*) flag);
    } else {
        simpleserial_put('r', 17, (uint8_t*) false_flag);
    }

    return (match_count == SEQUENCE_LENGTH) ? 0x01 : 0x00;
}

uint8_t adjust_thrust(uint8_t* data, uint8_t dlen) {
    uint8_t adjustment_value = 0;
    uint8_t resp = 1 ;
    queries ++ ;
    if (dlen == 1) {
        adjustment_value = data[0];
    }
    
    trigger_high();
    for (uint8_t i = 0; i < SEQUENCE_LENGTH; i++) {
        calibration_sequence[i] += adjustment_value;
    }
    trigger_low();
    
    simpleserial_put('r', 1, &resp);
    return 0;
}

uint8_t adjust_mixture(uint8_t* data, uint8_t dlen) {
    uint8_t mixture_factor = 1;
    uint8_t resp = 1 ;
    queries ++ ;
    if (dlen == 1) {
        mixture_factor = data[0];
    }

    trigger_high();
    for (uint8_t i = 0; i < SEQUENCE_LENGTH; i++) {
        calibration_sequence[i] *= mixture_factor;
    }
    trigger_low();
    
    simpleserial_put('r', 1, &resp);
    return 0;
}

void simple_delay_separator() {
    for (int i = 0; i < 25; i++) {
        __asm("nop");
    }
}

uint8_t invert_polarity(uint8_t* data, uint8_t dlen) {
    uint8_t inversion_mask = 0;
    uint8_t resp = 1 ;
    queries ++ ;
    if (dlen == 1) {
        inversion_mask = data[0];
    }

    trigger_high();

    volatile uint8_t* secret_bytes = (uint8_t*)secret_ignition_sequence;
    int total_bytes = sizeof(secret_ignition_sequence);
    volatile uint8_t temp_result ;

    for (int i = 0; i < total_bytes; i++) {
        temp_result = inversion_mask ^ secret_bytes[i]; 
        simple_delay_separator();
    }
    total_bytes += temp_result ;

    trigger_low();
    simpleserial_put('r', 1, &resp);
    
    return 0;
}

uint8_t num_q(uint8_t* data, uint8_t dlen)
{
        simpleserial_put('r', 4, (uint8_t*) &queries);
        return 0 ;
}


int main(void) {
    platform_init();
    init_uart();
    trigger_setup();
    simpleserial_init();

    initialize_sequences();

    simpleserial_addcmd('a', 12, arm_system);          // 'a' for arm
    simpleserial_addcmd('t', 1, adjust_thrust);       // 't' for thrust
    simpleserial_addcmd('m', 1, adjust_mixture);      // 'm' for mixture
    simpleserial_addcmd('p', 1, invert_polarity);     // 'p' for polarity
    simpleserial_addcmd('q', 1, num_q);     	      // 'q' num queries

    while(1) {
        simpleserial_get();
    }
    
    return 0;
}
