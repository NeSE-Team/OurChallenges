#include "stdio.h"
#include "time.h"
#include "openssl/md5.h"
int cmpmd5(int pData,char ans[])
{
    int i;
    //21232f297a57a5a743894a0e4a801fc3
    // unsigned char decrypt[16]; 
    char md5s[33]; 
    unsigned char md5[17]={0};  
    MD5_CTX c;
    MD5_Init(&c);            
    MD5_Update(&c, (char *)&pData, 4);
    MD5_Final(md5,&c);       
    for(i=0;i<16;i++)
    {
        sprintf(md5s+i*2,"%02x",md5[i]);
    }
    if(!strcmp(md5s,ans)){
        printf("%d",pData);
        return 1;
    }
    return 0;
}

int main(int argc,char *argv[]){
    int t=time(0);
    int t2=t; //Considering time zone
    while(1){
        srand(t);
        if(cmpmd5(rand(),argv[1]))break;
        t--;
        t2++;
        srand(t2);
        if(cmpmd5(rand(),argv[1]))break;
    }

    return 0;
}