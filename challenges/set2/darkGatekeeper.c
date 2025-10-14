#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "simpleserial.h"

#define KEY_LENGTH 12
#define RESPONSE_LENGTH 18

static const uint8_t flag[RESPONSE_LENGTH] ; //REDACTED
static const char master_key[KEY_LENGTH] ;  //REDACTED

uint8_t authenticate(uint8_t* incoming_key, uint8_t len)
{
    uint8_t msg_failure[RESPONSE_LENGTH] = "Access Denied.....";
    
    char access_granted = 1;
    int i;
    trigger_high();

    for (i = 0; i < KEY_LENGTH; i++) {
        if (incoming_key[i] != master_key[i]) 
	{
            access_granted = 0 ;
        }
    }

    trigger_low();

    if (access_granted) 
    {
        simpleserial_put('r', RESPONSE_LENGTH, flag);
    } else {
        simpleserial_put('r', RESPONSE_LENGTH, msg_failure);
    }
    return 0x00;
}

int main(void)
{
    platform_init();
    init_uart();
    trigger_setup();
    simpleserial_init();
    
    simpleserial_addcmd('a', KEY_LENGTH, authenticate);

    while(1) {
        simpleserial_get();
    }
}
