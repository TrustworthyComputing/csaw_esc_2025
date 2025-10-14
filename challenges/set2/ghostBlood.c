#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "simpleserial.h"

#define ROUNDS 20

uint32_t queries = 0 ;
uint16_t rotls = 0 ;
uint16_t thresh = 0 ;
uint16_t ROTL(uint16_t a, uint8_t b)
{
	uint16_t ret ;
	rotls++ ;
	if (rotls  == thresh){  trigger_high() ; }

	if(b > 16){ return a ; }
	else if (a >= (1<<(16-b)))
	{
		ret = ((a) << (b)) | ((a) >> (16 - (b))) ;
	}
	else
	{
		ret = (a) << (b) ;
	}
	if (rotls  == thresh){  trigger_low() ; }
	return ret ;
}

void QR(uint16_t* a, uint16_t* b, uint16_t* c, uint16_t* d, uint8_t shifts[4])
{
	b[0] ^= ROTL(*a + *d, shifts[0]) ;
	c[0] ^= ROTL(*b + *a, shifts[1]) ; 
	d[0] ^= ROTL(*c + *b, shifts[2]) ;
	a[0] ^= ROTL(*d + *c, shifts[3]) ;
}	

void block(uint16_t out[16], uint16_t const in[16], uint8_t shifts[4])
{
	int i;
	uint16_t x[16];

	for (i = 0; i < 16; ++i)
		x[i] = in[i];
	for (i = 0; i < ROUNDS; i += 2) 
	{
		// Odd round
		QR(&x[ 0], &x[ 4], &x[ 8], &x[12], shifts); // column 1
		QR(&x[ 5], &x[ 9], &x[13], &x[ 1], shifts); // column 2
		QR(&x[10], &x[14], &x[ 2], &x[ 6], shifts); // column 3
		QR(&x[15], &x[ 3], &x[ 7], &x[11], shifts); // column 4
		// Even round
		QR(&x[ 0], &x[ 1], &x[ 2], &x[ 3], shifts); // row 1
		QR(&x[ 5], &x[ 6], &x[ 7], &x[ 4], shifts); // row 2
		QR(&x[10], &x[11], &x[ 8], &x[ 9], shifts); // row 3
		QR(&x[15], &x[12], &x[13], &x[14], shifts); // row 4
	}
	for (i = 0; i < 16; ++i)
		out[i] = x[i] + in[i];
}

uint8_t dflag[16];        //REDACTED 
static uint16_t key[8] ; //REDACTED
static uint16_t nonce[4] = {    0xeaee,  0x83a0, 0xd9a6, 0xb8f7};


uint8_t shift(uint8_t* data, uint8_t dlen)
{
    uint16_t x[16] ;
    uint16_t y[16] ;
    uint8_t resp = 1;
    uint8_t* shifts = &data[2] ;
    thresh = data[0] + data[1]*256 ;
    rotls = 0 ;
    queries++ ;

    x[0] = 0x4554 ;
    x[1] = key[0] ;
    x[2] = key[1] ;
    x[3] = key[2] ;
    x[4] = key[3] ;
    x[5] = 0x4332 ;
    x[6] = nonce[0] ;
    x[7] = nonce[1] ;
    x[8] = nonce[2] ;
    x[9] = nonce[3] ;
    x[10] = 0x3032 ;
    x[11] = key[4] ;
    x[12] = key[5] ;
    x[13] = key[6] ;
    x[14] = key[7] ;
    x[15] = 0x3520 ;


    block(y, x, shifts) ;
    for(uint16_t i = 0 ; i < 16; i++) { y[i] = 0 ; x[i] = 0 ; }
    simpleserial_put('r', 1, &resp);
    return 0 ;
}


uint8_t decrypt(uint8_t* data, uint8_t dlen)
{
    uint16_t mykey[16] ;
    uint8_t flag[21] ;
    uint16_t x[16];
    uint16_t y[16];
    uint8_t shifts[4] = {2, 7, 9, 13} ;
    queries++ ;
    
    for(uint8_t i =0 ; i< 8; i++)
    {
    	mykey[i] = data[i*2] + data[i*2+1]*256 ;
    }

    x[0] = 0x4554 ;
    x[1] = mykey[0] ;
    x[2] = mykey[1] ;
    x[3] = mykey[2] ;
    x[4] = mykey[3] ;
    x[5] = 0x4332 ;
    x[6] = nonce[0] ;
    x[7] = nonce[1] ;
    x[8] = nonce[2] ;
    x[9] = nonce[3] ;
    x[10] = 0x3032 ;
    x[11] = mykey[4] ;
    x[12] = mykey[5] ;
    x[13] = mykey[6] ;
    x[14] = mykey[7] ;
    x[15] = 0x3520 ;

    block(y, x, shifts) ;
    flag[0] = 'E' ;
    flag[1] = 'S' ;
    flag[2] = 'C' ;
    flag[3] = '{' ;
    flag[20] = '}' ;
    for(uint8_t i = 0 ; i< 16; i++)
    {
	flag[4+i] = y[i] ^ dflag[i] ;
    }

    simpleserial_put('r', 21, flag);
    return 0 ;
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

    simpleserial_addcmd('s', 6, shift);
    simpleserial_addcmd('d', 16, decrypt);
    simpleserial_addcmd('q', 1, num_q);
    while(1) simpleserial_get();
    
    return 0;
}
