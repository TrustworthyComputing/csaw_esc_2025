#include "hal.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "../simpleserial/simpleserial.h"

#define DELTA 0x9e3779b9
#define MX (((z >> 5 ^ y << 2) + (y >> 3 ^ z << 4)) ^ ((sum ^ y) + (key[(p & 3) ^ e] ^ z)))

static const uint8_t flag[17] ; //REDACTED
static const uint32_t xxtea_key[4] ; //REDACTED

void xxtea_encrypt(uint32_t* v, int n, const uint32_t key[4]) {
    uint32_t y, z, sum;
    unsigned p, rounds, e;
    if (n <= 1) { return; }
    
    rounds = 6 + 52 / n;
    sum = 0;
    z = v[n - 1];
    
    do {
        sum += DELTA;
        e = (sum >> 2) & 3;

        for (p = 0; p < n - 1; p++) {
            y = v[p + 1];
            z = v[p] += MX;
        }
        y = v[0];
        z = v[n - 1] += MX;
        
    } while (--rounds);
}

void xxtea_decrypt(uint32_t* v, int n, const uint32_t key[4]) {
    uint32_t y, z, sum;
    unsigned p, rounds, e;
    if (n <= 1) { return; }
    rounds = 6 + 52 / n;
    sum = rounds * DELTA;
    y = v[0];
    do {
        e = (sum >> 2) & 3;
        for (p = n - 1; p > 0; p--) {
            z = v[p - 1];
            y = v[p] -= MX;
        }
        z = v[n - 1];
        y = v[0] -= MX;
        sum -= DELTA;
    } while (--rounds);
}
void delay(uint32_t count) {
    for (uint32_t i = 0; i < count; ++i) {
        __asm__("nop");
    }
}

uint8_t encrypt(uint8_t* data, uint8_t dlen) {
  if (dlen < 8 || dlen % 4 != 0){ return 1; }

  trigger_high();
  volatile uint8_t temp;
  for (int i = 0; i < 16; i++) {
    temp = data[i % dlen] ^ ((uint8_t*)xxtea_key)[i];
    delay(15);
    data[i%dlen] = temp ;
  }
  trigger_low();

  uint32_t* data_words = (uint32_t*)data;
  int n = dlen / 4 ;
  xxtea_encrypt(data_words, n, xxtea_key);
  simpleserial_put('r', dlen, data);
  return 0x00;
}

uint8_t decrypt(uint8_t* data, uint8_t dlen) 
{
  if (dlen < 8 || dlen % 4 != 0) return 1;
  
  uint32_t* data_words = (uint32_t*)data;
  int n = dlen / 4;
  xxtea_decrypt(data_words, n, xxtea_key);

  volatile uint8_t temp;
  for (int i = 0; i < 16; i++) {
    temp = data[i % dlen] ^ ((uint8_t*)xxtea_key)[i];
    data[i%dlen] = temp ;
  }
 
  simpleserial_put('r', dlen, data);
  return 0x00;
}

uint8_t verify(uint8_t* data, uint8_t dlen) {
  uint8_t dummy_flag [17] = "deadlyWhiteJade!!" ; 
  if (dlen != 16) 
  {
    simpleserial_put('r', 17, dummy_flag);
    return 1;
  }
  
  uint8_t matched = 0 ;
  for (int i = 0; i < 16; i++) {
    if (((uint8_t*)xxtea_key)[i] == data[i]) 
    {
	matched++ ;
    }
  }
  if(matched != 16)
  {
  	simpleserial_put('r', 17, dummy_flag);
  	return 0x01;
  }
  else
  {
  	simpleserial_put('r', 17, (uint8_t*)flag);
  	return 0x00;
  }
}

int main(void) {
  platform_init();
  init_uart();
  trigger_setup();
  simpleserial_init();

  simpleserial_addcmd('e', 8, encrypt);
  simpleserial_addcmd('d', 8, decrypt);
  simpleserial_addcmd('c', 16, verify);

  while(1) {
    simpleserial_get();
  }
  return 0;
}
