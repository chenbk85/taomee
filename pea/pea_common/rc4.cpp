#include "rc4.hpp"

void rc4_init(unsigned char* s_box, unsigned char* key, unsigned long key_long)
{
	int32_t i =0;
	int32_t j =0;
	int k[256] = {0};
	unsigned char temp = 0;

	for(i =0; i< 256; i++)
	{
		s_box[i] = i;
		k[i] = key[i % key_long];
	}

	for(i=0; i< 256; i++)
	{
		j = (j + s_box[i] + k[i]) % 256;
		temp = s_box[i];
		s_box[i] = s_box[j];
		s_box[j] = temp;
	}
}

void rc4_encrypt(unsigned char* s_box, unsigned char* data,  unsigned long data_len)
{
	int x = 0;
	int y = 0;
	int t = 0;
	unsigned int i = 0;

	for(i =0; i < data_len; i++)
	{
		x = (x + 1) % 256;
		y = (y + s_box[x]) % 256;
		t = (s_box[x] + s_box[y]) % 256;
		data[i] ^= s_box[t];
	}
}


/*
int main()
{
	unsigned char box[256] = {0};
	char key[256] = "just for test";
	char data[512] = "1234567890abcdefghijklmn";
	unsigned long data_len = strlen(data);

	rc4_init(box, (unsigned char*)key,  strlen(key));
	rc4_encrypt(box, (unsigned char*)data, data_len);
	rc4_decrypt(box, (unsigned char*)data, data_len);
	return 0;	
}
*/

