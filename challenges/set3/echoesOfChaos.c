#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "simpleserial.h"


#define ARR_LEN 15
#define FLAG_LEN 20
static const uint8_t dflag[FLAG_LEN] ; //REDACTED
uint16_t original_arr[ARR_LEN]; //REDACTED
static const uint8_t flag_len = sizeof(dflag);

uint16_t data_arr[ARR_LEN+1]; 
uint8_t current_array_length = ARR_LEN ;
uint8_t is_sorted = 0;
uint32_t queries = 0 ;

void clyde(uint16_t* sue, uint8_t inky, uint8_t blinky, uint8_t pinky) {
    uint8_t i, j, k;
    uint8_t n1 = blinky - inky + 1;
    uint8_t n2 = pinky - blinky;

    uint16_t casper[ARR_LEN/2+1], wendy[ARR_LEN/2+1];

    for (i = 0; i < n1; i++)
        casper[i] = sue[inky + i];
    for (j = 0; j < n2; j++)
        wendy[j] = sue[blinky + 1 + j];

    i = 0;
    j = 0;
    k = inky;
    while (i < n1 && j < n2) 
    {
        if (casper[i] <= wendy[j]) 
	{
            sue[k] = casper[i];
	    for(volatile uint8_t del = 0; del < 100+i ; del++) ;
            i++;
        }
        else {
            sue[k] = wendy[j];
            j++;
        }
        k++;
    }

    while (i < n1) 
    {
        sue[k] = casper[i];
	for(volatile uint8_t del = 0; del < 100+i ; del++) ;
        i++;
        k++;
    }

    while (j < n2) {
        sue[k] = wendy[j];
        j++;
        k++;
    }
}

void clydeSort(uint16_t* sue, uint8_t inky, uint8_t pinky) {
    if (inky < pinky) 
    {
      
        uint8_t blinky = inky + (pinky - inky) / 2 ;
        clydeSort(sue, inky, blinky);
        clydeSort(sue, blinky + 1, pinky);
        clyde(sue, inky, blinky, pinky);
    }
}

void initialize_original_arr() {
    for(uint8_t i = 0; i < ARR_LEN; i++) data_arr[i] = (uint16_t) (original_arr[i]) ;
}

uint8_t check_arr(uint8_t* arr, uint8_t len)
{
    if(queries == 0){ initialize_original_arr(); }
    queries ++ ;
    const char false_flag[] = "q3L!x9bX2f@mV#6dWrx\n";

    trigger_high();
    for(uint8_t i = 0; i < ARR_LEN; i++) data_arr[i] = (uint16_t) (original_arr[i]) ;
    clydeSort(data_arr, 0, ARR_LEN-1);
    is_sorted = 1 ;
    trigger_low();
    if (len != 30) 
    {
        simpleserial_put('r', sizeof(false_flag) - 1, (uint8_t*)false_flag);
        return 0x00;
    }
    
    uint8_t correct= 1 ;
    for (uint8_t i = 0; i < 15 ; i++) {
        uint16_t num = (uint16_t) arr[i*2] + (uint16_t) arr[i*2+1]*256 ;
	if (num != data_arr[i]) {
	    correct = 0 ;
        }
    }

    if(correct == 0)
    {
    	simpleserial_put('r', sizeof(false_flag) - 1, (uint8_t*)false_flag);
	return 0x00 ;
    }

    simpleserial_put('r', flag_len, flag);
    return 0x00;
}

uint8_t get_pt(uint8_t* pt, uint8_t len)
{
    uint8_t ret = 0;
    uint8_t elements_to_skip = pt[3] ; 
    if(queries == 0){ initialize_original_arr(); }
    if(is_sorted == 0)
    { 
    	queries++ ;
    	trigger_high();
    	uint16_t new_value = pt[1] + pt[2]*256 ;
    	if(elements_to_skip > ARR_LEN-1) elements_to_skip = ARR_LEN-1;
    	current_array_length = ARR_LEN - elements_to_skip + 1;
    
    	data_arr[0] = new_value;
   	for(uint8_t i = 0; i < (ARR_LEN - elements_to_skip); i++) 
	{ data_arr[i+1] = original_arr[i + elements_to_skip]; }
    	clydeSort(data_arr, 0, current_array_length-1);
    	is_sorted = 1;
    	trigger_low();
	ret = 1 ;
    }
    simpleserial_put('r', 1, &ret);
    return 0x00;
}

uint8_t reset(uint8_t* x, uint8_t len)
{
    if(queries == 0){ initialize_original_arr(); }
    current_array_length = ARR_LEN ;
    for(uint8_t i = 0; i < ARR_LEN; i++) data_arr[i] = original_arr[i];
    is_sorted = 0;
    uint8_t ret = 1; 
    simpleserial_put('r', 1, &ret);
    return 0x00;
}


uint8_t num_q(uint8_t* data, uint8_t dlen)
{
        simpleserial_put('r', 4, (uint8_t*) &queries);
        return 0 ;
}


int main(void)
{
    platform_init();
    init_uart();
    trigger_setup();
    simpleserial_init();
    
    simpleserial_addcmd('p', 4, get_pt);
    simpleserial_addcmd('a', 30, check_arr);
    simpleserial_addcmd('x', 0, reset);
    simpleserial_addcmd('q', 1, num_q);
    
    initialize_original_arr(); 
   
    while(1) simpleserial_get();
}
