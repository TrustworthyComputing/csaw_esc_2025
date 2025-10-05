#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "simpleserial.h"

static const uint8_t flag[] ; //REDACTED
static const uint8_t flag_len = sizeof(flag);

uint8_t run_diagnostic(uint8_t* in, uint8_t len)
{
    volatile uint16_t i, j;
    volatile uint32_t cnt;
    cnt = 0;

    trigger_high();
    for(i = 0; i < 100; i++){             
        for(j = 0; j < 40; j++){
            cnt += 2;
        }
    }
    trigger_low();

    if (cnt != 8000) {
        simpleserial_put('r', flag_len, flag);

    } else {
        uint8_t ok_msg[] = "DIAGNOSTIC_OK             ";
        simpleserial_put('r', flag_len, (uint8_t*)&ok_msg);
    }
    return (cnt != 8000) ? 0x10 : 0x00;
}

int main(void)
{
    platform_init();
    init_uart();
    trigger_setup();

    simpleserial_init();
    simpleserial_addcmd('d', 0, run_diagnostic);
    while(1)
        simpleserial_get();
}
