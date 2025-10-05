#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "simpleserial.h"


static const uint8_t password_len[2] = {8+5, 12+5} ;
static uint32_t queries = 0 ;

uint8_t verify(uint8_t* data, uint8_t dlen) {
    const uint8_t password[2][32] ; //REDACTED
    uint8_t correct = 0;
    uint8_t matched_chars = 0;
    uint8_t c = 0 ;
    if(data[2] == '2'){ c = 1 ; } 
    queries++ ;
    	// Will return None
    if (dlen != password_len[c]) { correct = 0xff ; }
    	// Will return 0xfe
    else if(data[0] != 'g' || data[1] != 'k' || data[2] != '1'+c || 
	data[3] != '{' || data[dlen-1] != '}')
    { 	correct = 0xfe ; }
 

    trigger_high();
    
    for(uint8_t i = 0; i < password_len[c]; i++) {
        if(data[i] == password[c][i]) {
            matched_chars++;
            for(volatile uint32_t j = 0; j < (uint32_t) (5000 / (c+1)) - i*125; j++);
        } else {
            break;
        }
    }
    
    trigger_low();

    if(matched_chars == password_len[c]) { correct = 1; }

    simpleserial_put('r', 1, &correct);
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

    simpleserial_addcmd('a', password_len[0], verify);
    simpleserial_addcmd('b', password_len[1], verify);
    simpleserial_addcmd('q', 1, num_q);
    while(1) simpleserial_get();
    
    return 0;
}
