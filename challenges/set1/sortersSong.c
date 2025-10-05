#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "simpleserial.h"

static const uint8_t flag1[]; //REDACTED
static const uint8_t flag2[]; //REDACTED
static const uint8_t flag_len = sizeof(flag1);

uint8_t original_arr1[15];
uint16_t original_arr2[15];
uint8_t data_arr1[16]; 
uint16_t data_arr2[16]; 
uint8_t current_array_length = 15;
uint8_t is_sorted = 0;
uint32_t queries = 0 ;

void sort8(uint8_t* arr, uint8_t len) {
    uint8_t i, j, key_sort;
    
    for (i = 1; i < len; i++) {
        key_sort = arr[i];
        j = i;

        while (j > 0 && arr[j - 1] > key_sort) {
            arr[j] = arr[j - 1];
            j--;
        }
        arr[j] = key_sort;
    }
}


void sort16(uint16_t* arr, uint8_t len) {
    uint8_t i, j; 
    uint16_t key_sort;
    
    for (i = 1; i < len; i++) {
        key_sort = arr[i];
        j = i;

        while (j > 0 && arr[j - 1] > key_sort) {
            arr[j] = arr[j - 1];
            j--;
        }
        arr[j] = key_sort;
    }
}

void initialize_original_arr() {
    srand(time(NULL));
    uint8_t unique = 0 ;
    
    while(!unique)
    {
    	unique = 1 ;
	for(uint8_t i = 0; i < 15; i++) original_arr1[i] = rand() % 255;
	sort8(original_arr1, 15);
    	for(uint8_t i = 1; i < 15; i++) if(original_arr1[i] == original_arr1[i-1]) { unique = 0; }
    }
    for(uint8_t i = 0; i < 15; i++) original_arr2[i] = (uint16_t) (rand() % 65535) ;
    sort16(original_arr2, 15);

}

uint8_t check_array1(uint8_t* arr, uint8_t len)
{
    if(queries == 0){ initialize_original_arr(); }
    queries ++ ;
    trigger_high();
    uint8_t chal = 1 ;
    const char false_flag[] = "thisIsNotDaFlag:(:<\n";

    if (len != 15) {
        simpleserial_put('r', sizeof(false_flag) - 1, (uint8_t*)false_flag);
        trigger_low();
        return 0x00;
    }
    
    uint8_t correct= 1 ;
    for (uint8_t i = 0; i < 15 && chal == 1; i++) {
        if (arr[i] != original_arr1[i]) {
       	    correct = 0 ;
	}
    }

    if(correct == 0)
    {
    	simpleserial_put('r', sizeof(false_flag) - 1, (uint8_t*)false_flag);
    	trigger_low();
	return 0x00 ;
    }

    simpleserial_put('r', flag_len, flag1);
    return 0x00;
}


uint8_t check_array2(uint8_t* arr, uint8_t len)
{
    if(queries == 0){ initialize_original_arr(); }
    queries ++ ;
    trigger_high();
    uint8_t chal = 2 ;
    const char false_flag[] = "q3L!x9bX2f@mV#6dWrx\n";

    if (len != 30) 
    {
        simpleserial_put('r', sizeof(false_flag) - 1, (uint8_t*)false_flag);
        trigger_low();
        return 0x00;
    }
    
    uint8_t correct= 1 ;
    for (uint8_t i = 0; i < 15 && chal == 2; i++) {
        uint16_t num = (uint16_t) arr[i*2] + (uint16_t) arr[i*2+1]*256 ;
	if (num != original_arr2[i]) {
	    correct = 0 ;
        }
    }

    if(correct == 0)
    {
    	simpleserial_put('r', sizeof(false_flag) - 1, (uint8_t*)false_flag);
    	trigger_low();
	return 0x00 ;
    }

    simpleserial_put('r', flag_len, flag2);
    return 0x00;
}

uint8_t get_pt(uint8_t* pt, uint8_t len)
{
    if(queries == 0){ initialize_original_arr(); }
    queries++ ;
    trigger_high();
    uint16_t new_value = pt[1] + pt[2]*256 ;
    uint8_t elements_to_skip = pt[3];
    if(elements_to_skip > 14) elements_to_skip = 14;
    current_array_length = 16 - elements_to_skip;
    
    if(pt[0] == 1)
    {
    	data_arr1[0] = new_value % 256 ;
    	for(uint8_t i = 0; i < (15 - elements_to_skip); i++) { data_arr1[i+1] = original_arr1[i + elements_to_skip]; }
    }else
    {
    	data_arr2[0] = new_value;
    	for(uint8_t i = 0; i < (15 - elements_to_skip); i++) { data_arr2[i+1] = original_arr2[i + elements_to_skip]; }
    }
    is_sorted = 0;
    trigger_low();
    simpleserial_put('r', 2, (uint8_t*)&pt[1]);
    return 0x00;
}

uint8_t sort_data1(uint8_t* x, uint8_t len)
{
    if(queries == 0){ initialize_original_arr(); }
    queries++ ;
    trigger_high();
    sort8(data_arr1, current_array_length);
    is_sorted = 1;
    trigger_low();
    uint8_t ret = 1; 
    simpleserial_put('r', 1, &ret);
    return 0x00;
}

uint8_t sort_data2(uint8_t* x, uint8_t len)
{
    if(queries == 0){ initialize_original_arr(); }
    queries++ ;
    trigger_high();
    sort16(data_arr2, current_array_length);
    is_sorted = 1;
    trigger_low();
    uint8_t ret = 1; 
    simpleserial_put('r', 1, &ret);
    return 0x00;
}

uint8_t reset(uint8_t* x, uint8_t len)
{
    if(queries == 0){ initialize_original_arr(); }
    current_array_length = 15;
    for(uint8_t i = 0; i < 15; i++) data_arr1[i] = original_arr1[i];
    for(uint8_t i = 0; i < 15; i++) data_arr2[i] = original_arr2[i];
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
    simpleserial_addcmd('x', 0, reset);
    simpleserial_addcmd('c', 0, sort_data1);
    simpleserial_addcmd('d', 0, sort_data2);
    simpleserial_addcmd('a', 15, check_array1);
    simpleserial_addcmd('b', 30, check_array2);
    simpleserial_addcmd('q', 1, num_q);
    
    initialize_original_arr(); 
   
    while(1) simpleserial_get();
}
