#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "gen.h"


char flag[] = "flag{The_Auth0r_was_stark_mad_and_keep_listening_surfin_bird_for_comfort}";

struct fragment fragment_list[] = {
    {FIBNACCI, 4, {0xf4c2fe42, 0x1d6999b, 0xb518ef62, 0x6197ecb}}, 
    {XORSTRING, 3, {0x36, 0x49, 0xb}, }, 
    {CRC32, 2, {0x83d3e024}}, 
    {FIBNACCI, 5, {0x35c7e2L, 0xe4626510L, 0xb518ef62L, 0x1d6999bL, 0x198c09a0L}}, 
    {BASE64, 2, {0x33, 0x35, 0x37, 0x3d}}, 
    {BASE64, 6, {0x6e, 0x54, 0x64, 0x46, 0x67, 0x39, 0x2b, 0x43}}, 
    {XORSTRING, 2, {0x2a, 0xc}}, 
    {XORSTRING, 5, {0x62, 0xb, 0x1b, 0x16, 0x48}}, 
    {BASE64, 5, {0x77, 0x42, 0x2b, 0x36, 0x67, 0x54, 0x72, 0x3d}}, 
    {XORSTRING, 2, {0x30, 0x10}}, 
    {FIBNACCI, 4, {0xcafb7902L, 0x1d6999bL, 0x909038c5L, 0x8bf6d4f1L}}, 
    {BASE64, 4, {0x33, 0x35, 0x4a, 0x48, 0x69, 0x45, 0x3d, 0x3d}}, 
    {BASE64, 6, {0x67, 0x4c, 0x7a, 0x63, 0x67, 0x35, 0x62, 0x62}}, 
    {FIBNACCI, 6, {0x983179c2L, 0x35c7e2L, 0x983179c2L, 0x238f5b22L, 0x5624f888L, 0x35c7e2L}}, 
    {FIBNACCI, 4, {0xcafb7902L, 0x606984f7L, 0x6fb5ab95L, 0x909038c5L}}, 
    {FIBNACCI, 5, {0x35c7e2L, 0x606984f7L, 0x287bce2dL, 0x35c7e2L, 0x983179c2L}}, 
    {BASE64, 4, {0x6a, 0x35, 0x72, 0x63, 0x67, 0x45, 0x3d, 0x3d}}, 
    {FIBNACCI, 4, {0x1d6999bL, 0x6fb5ab95L, 0x909038c5L, 0x5704e7L}}
};

char ss[18][10] = {"Don'", "t y", "ou", " know", " a", "bout t", "he", " bird", "? Eve", "ry", "body", " kno", "ws tha", "t the ", "bird", " is t", "he w", "ord!"};


char *changeTable(struct fragment *frag_l){
	for(int i=0;i<18;i++){
        int pos1 = i;
		// int pos1 = ((unsigned)rand() * 100) % 18;
		int pos2 = ((unsigned)rand() * 100) % 18;
		struct fragment tmp = frag_l[pos1];
		frag_l[pos1] = frag_l[pos2];
		frag_l[pos2] = tmp;    
        char stmp[10] = {0};
        memcpy(stmp, ss[pos1], 10);
        memset(ss[pos1], 0, 10);
        memcpy(ss[pos1], ss[pos2], 10);
        memset(ss[pos2], 0, 10);        
        memcpy(ss[pos2], stmp, 10);
    }
    char *tran_str = (char *)malloc(100);
    for(int i=0;i<18;i++){
        strncat(tran_str, ss[i], strlen(ss[i]));
    }
    return tran_str;

}

void xorflag(char *flag, char *input){
	for(int i=0; i<73; i++){
		flag[i] ^= input[i];
	}
}

void genencryptflag(){
    srand(42);

    char src_in[80] = "Don't you know about the bird? Everybody knows that the bird is the word!";
    xorflag(flag, src_in);

    for (int i=1; i<233; i++){
		char *input = changeTable(fragment_list);
		xorflag(flag, input);
        printf("%3d: %s\n", i, input);
        free(input);
	}
    
    for(int i=0;i<73;i++){
        printf("%d, ", flag[i]);
    }
}

void gencheckdata(){
    unsigned char input[80] = "Don't you know about the bird? Everybody knows that the bird is the word!";

    int start = 0;

    for(int i=0;i<18;i++){
        struct fragment frag = fragment_list[i];
        unsigned char data[10] = {0};
        memcpy(data, &input[start], frag.inputlen);
        start += frag.inputlen;
        switch (frag.func)
        {
        case CRC32:
            gencrc32(data);
            break;
        case BASE64:
            genbase64(data);
            break;
        case FIBNACCI:
            genfibnacci(data);
            break;
        case XORSTRING:
            genxorstring(data);
            break;
        default:
            break;
        }
    }
}

int main(){

    gencheckdata();
    // genencryptflag();

    return 0;
}