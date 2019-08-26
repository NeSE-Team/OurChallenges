
#ifndef _FRAGMENT_H_
#define _FRAGMENT_H_

enum check_type{
    CRC32,
    FIBNACCI,
    BASE64,
    XORSTRING,
};

struct fragment{
    enum check_type func;
    int inputlen;
    unsigned int checkdata[16];
};


void gencrc32(unsigned char *in);
void genfibnacci(unsigned char *in);
void genbase64(unsigned char *in);
void genxorstring(unsigned char *in);

#endif