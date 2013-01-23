#include <stdio.h>
#include <string.h>
#include <stdint.h>


void rc4_init(unsigned char* s_box, unsigned char* key, unsigned long key_long);
void rc4_encrypt(unsigned char* s_box, unsigned char* data,  unsigned long data_len);
#define rc4_decrypt  rc4_encrypt

