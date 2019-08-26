#include "gen.h"
#include <stdio.h>


#define TRUE 1
#define FALSE 0

int my_strlen(unsigned char *str){
    int i = 0;
    for (; str[i] != '\0'; i++){}
    return i;
}


unsigned int crc32b(unsigned char *message) {
   int i, j;
   unsigned int byte, crc, mask;

   i = 0;
   crc = 0xFFFFFFFF;
   while (message[i] != 0) {
      byte = message[i];            // Get next byte.
      crc = crc ^ byte;
      for (j = 7; j >= 0; j--) {    // Do eight times.
         mask = -(crc & 1);
         crc = (crc >> 1) ^ (0xEDB88320 & mask);
      }
      i = i + 1;
   }
   return ~crc;
}


unsigned int fibnacci(unsigned char ch){
    unsigned long long a = 1;
    unsigned long long b = 0;
    unsigned long long tmp;

    for(int i = 0; i < ch; i++){
        tmp = a + b;
        b = a;
        a = tmp;
    }

    return a & 0xffffffff;

}



#define BASE64_PAD '='

unsigned int base64_encode(const unsigned char *in, unsigned int inlen, char *out)
{
	int s;
	unsigned int i;
	unsigned int j;
	unsigned char c;
	unsigned char l;
    // const unsigned char base64en[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    unsigned char base64en[] = "zy0xw1vu2ts3rq4po5nm6lk7ji8hg9fedcba,.ZYXWVUTSRQPONMLKJIHGFEDCBA";

	s = 0;
	l = 0;
	for (i = j = 0; i < inlen; i++) {
		c = in[i];

		switch (s) {
		case 0:
			s = 1;
			out[j++] = base64en[(c >> 2) & 0x3F];
			break;
		case 1:
			s = 2;
			out[j++] = base64en[((l & 0x3) << 4) | ((c >> 4) & 0xF)];
			break;
		case 2:
			s = 0;
			out[j++] = base64en[((l & 0xF) << 2) | ((c >> 6) & 0x3)];
			out[j++] = base64en[c & 0x3F];
			break;
		}
		l = c;
	}

	switch (s) {
	case 1:
		out[j++] = base64en[(l & 0x3) << 4];
		out[j++] = BASE64_PAD;
		out[j++] = BASE64_PAD;
		break;
	case 2:
		out[j++] = base64en[(l & 0xF) << 2];
		out[j++] = BASE64_PAD;
		break;
	}

	out[j] = 0;

	return j;
}


unsigned int xor_string(unsigned char *in, int len, char *out){
    
    unsigned char str[] = "Bird, Bird, Bird, Bird is the word.";

    for (int i = 0; i < len; i++){
        out[i] = in[i] ^ str[i % 35];
    }
    
    return len;
}



void gencrc32(unsigned char *in){
    
    printf("crc32(%s): ", in);
    printf("0x%x\n", crc32b(in));
}


void genfibnacci(unsigned char *in){
    printf("fibnacci(%s): ", in);
    for (int i=0; i<my_strlen(in); i++){
        printf("0x%x, ", fibnacci(in[i]));
    }
    puts("");
}


void genbase64(unsigned char *in){
    printf("s_base64(%s): ", in);
    char out[32];
    int encode_len = base64_encode(in, my_strlen(in), out);
    
    for (int i = 0; i<encode_len; i++){
        printf("0x%x, ", out[i]);
    } 
    puts("");
}

void genxorstring(unsigned char *in){
    printf("xorstring(%s): ", in);
    char out[32];
    int encode_len = xor_string(in, my_strlen(in), out);

    for (int i=0; i<encode_len; i++){
        printf("0x%x, ", out[i]);        
    }
    puts("");
}