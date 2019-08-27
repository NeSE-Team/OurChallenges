#include "stdio.h"
#include "time.h"
#include "stdlib.h"
#include "math.h"
#define Nr 16
#define l  0x140000    //分组数量文件大小为l*x位数
typedef union eight_unsigned{//定义联合
	unsigned long long val;
	struct{
		unsigned u8:8; //8bit一个S盒
		unsigned u7:8;
		unsigned u6:8;
		unsigned u5:8;
		unsigned u4:8;
		unsigned u3:8;
		unsigned u2:8;
		unsigned u1:8;
	}us;
	struct{
		unsigned b64:1;//64位bit
		unsigned b63:1;
		unsigned b62:1;
		unsigned b61:1;
		unsigned b60:1;
		unsigned b59:1;
		unsigned b58:1;
		unsigned b57:1;
		unsigned b56:1;
		unsigned b55:1;
		unsigned b54:1;
		unsigned b53:1;
		unsigned b52:1;
		unsigned b51:1;
		unsigned b50:1;
		unsigned b49:1;
		unsigned b48:1;
		unsigned b47:1;
		unsigned b46:1;
		unsigned b45:1;
		unsigned b44:1;
		unsigned b43:1;
		unsigned b42:1;
		unsigned b41:1;
		unsigned b40:1;
		unsigned b39:1;
		unsigned b38:1;
		unsigned b37:1;
		unsigned b36:1;
		unsigned b35:1;
		unsigned b34:1;
		unsigned b33:1;
		unsigned b32:1;
		unsigned b31:1;
		unsigned b30:1;
		unsigned b29:1;
		unsigned b28:1;
		unsigned b27:1;
		unsigned b26:1;
		unsigned b25:1;
		unsigned b24:1;
		unsigned b23:1;
		unsigned b22:1;
		unsigned b21:1;
		unsigned b20:1;
		unsigned b19:1;
		unsigned b18:1;
		unsigned b17:1;
		unsigned b16:1;
		unsigned b15:1;
		unsigned b14:1;
		unsigned b13:1;
		unsigned b12:1;
		unsigned b11:1;
		unsigned b10:1;
		unsigned b9 :1;
		unsigned b8 :1;
		unsigned b7 :1;
		unsigned b6 :1;
		unsigned b5 :1;
		unsigned b4 :1;
		unsigned b3 :1;
		unsigned b2 :1;
		unsigned b1 :1;
	}bs;
}eight_unsigned;
unsigned int SBOX[16][16]={{ 0x0b , 0xc9 , 0xac , 0xa6 , 0x63 , 0xf3 , 0xfe , 0xa3 , 0xe0 , 0x27 , 0x22 , 0xce , 0xee , 0x00 , 0xca , 0x34 },
{ 0x15 , 0x4a , 0x8a , 0x9a , 0xa2 , 0x36 , 0xed , 0x5e , 0xba , 0xd8 , 0xf9 , 0x76 , 0x65 , 0x31 , 0x55 , 0xc3 },
{ 0xcc , 0xc1 , 0x0e , 0x1b , 0xe4 , 0x7a , 0x43 , 0x54 , 0x95 , 0x6f , 0xfa , 0x61 , 0xbe , 0x1e , 0xe2 , 0x46 },
{ 0x4d , 0x49 , 0xa7 , 0x2b , 0xd1 , 0x0f , 0x2a , 0x58 , 0xc4 , 0xad , 0x05 , 0x42 , 0x33 , 0x78 , 0xea , 0x72 },
{ 0x6e , 0xae , 0xab , 0xe6 , 0xf2 , 0x04 , 0x96 , 0x90 , 0x66 , 0x0a , 0x01 , 0x40 , 0x8d , 0xaf , 0x23 , 0x57 },
{ 0xb2 , 0xc5 , 0x41 , 0xaa , 0xb3 , 0x92 , 0x70 , 0xdc , 0x2d , 0xb8 , 0x47 , 0x94 , 0x6d , 0x1c , 0xef , 0x12 },
{ 0x38 , 0xc7 , 0xb7 , 0x18 , 0xbd , 0x87 , 0xb6 , 0x91 , 0x13 , 0x50 , 0xf7 , 0x4b , 0x67 , 0xd5 , 0x64 , 0x68 },
{ 0x4e , 0x75 , 0xa4 , 0x7b , 0xbb , 0x4f , 0x8e , 0x7c , 0xf0 , 0x5a , 0x74 , 0xd7 , 0x35 , 0xd4 , 0x7f , 0xf4 },
{ 0xcd , 0x83 , 0x2c , 0x30 , 0xd6 , 0x5f , 0x1a , 0x2e , 0x24 , 0xc6 , 0x71 , 0x56 , 0x11 , 0xff , 0xc2 , 0x0c },
{ 0x93 , 0x99 , 0xe1 , 0x21 , 0x77 , 0x1f , 0xe3 , 0xa0 , 0x3d , 0x29 , 0x5c , 0xe9 , 0x84 , 0x9c , 0x8f , 0x10 },
{ 0x17 , 0xb1 , 0x7e , 0x48 , 0xf6 , 0xb4 , 0xe5 , 0x6b , 0xde , 0xd0 , 0x20 , 0x86 , 0xcb , 0xda , 0x85 , 0x14 },
{ 0x82 , 0x16 , 0x52 , 0x5b , 0xb5 , 0xbc , 0x44 , 0x02 , 0x07 , 0xdb , 0xf5 , 0x19 , 0x7d , 0xd2 , 0x39 , 0x9e },
{ 0x3e , 0x32 , 0x5d , 0x06 , 0xf8 , 0x6c , 0xeb , 0x6a , 0x08 , 0xdf , 0x2f , 0x79 , 0x4c , 0x8b , 0xcf , 0x81 },
{ 0x88 , 0xd9 , 0x73 , 0xe8 , 0x89 , 0xec , 0x3a , 0x59 , 0x25 , 0x3f , 0x3c , 0x97 , 0xfb , 0x69 , 0x53 , 0x0d },
{ 0xfd , 0xe7 , 0xc8 , 0x60 , 0x3b , 0x80 , 0x51 , 0x8c , 0x28 , 0x03 , 0xf1 , 0xfc , 0xbf , 0x45 , 0x98 , 0xa5 },
{ 0x1d , 0x26 , 0xa8 , 0xc0 , 0x9b , 0xa9 , 0xb0 , 0xa1 , 0x9f , 0xdd , 0x09 , 0x9d , 0x37 , 0xd3 , 0x62 , 0xb9 }};
unsigned int S(unsigned int in)//代换函数
{
	return SBOX[in>>16][in&0xFFFF];
}
unsigned long long P(eight_unsigned in)//置换函数
{
	eight_unsigned last;
	last.bs.b1 =in.bs.b1 ;last.bs.b9 =in.bs.b2 ;last.bs.b17=in.bs.b3 ;last.bs.b25=in.bs.b4 ;last.bs.b33=in.bs.b5 ;last.bs.b41=in.bs.b6 ;last.bs.b49=in.bs.b7 ;last.bs.b57=in.bs.b8 ;
	last.bs.b2 =in.bs.b9 ;last.bs.b10=in.bs.b10;last.bs.b18=in.bs.b11;last.bs.b26=in.bs.b12;last.bs.b34=in.bs.b13;last.bs.b42=in.bs.b14;last.bs.b50=in.bs.b15;last.bs.b58=in.bs.b16;
	last.bs.b3 =in.bs.b17;last.bs.b11=in.bs.b18;last.bs.b19=in.bs.b19;last.bs.b27=in.bs.b20;last.bs.b35=in.bs.b21;last.bs.b43=in.bs.b22;last.bs.b51=in.bs.b23;last.bs.b59=in.bs.b24;
	last.bs.b4 =in.bs.b25;last.bs.b12=in.bs.b26;last.bs.b20=in.bs.b27;last.bs.b28=in.bs.b28;last.bs.b36=in.bs.b29;last.bs.b44=in.bs.b30;last.bs.b52=in.bs.b31;last.bs.b60=in.bs.b32;
	last.bs.b5 =in.bs.b33;last.bs.b13=in.bs.b34;last.bs.b21=in.bs.b35;last.bs.b29=in.bs.b36;last.bs.b37=in.bs.b37;last.bs.b45=in.bs.b38;last.bs.b53=in.bs.b39;last.bs.b61=in.bs.b40;
	last.bs.b6 =in.bs.b41;last.bs.b14=in.bs.b42;last.bs.b22=in.bs.b43;last.bs.b30=in.bs.b44;last.bs.b38=in.bs.b45;last.bs.b46=in.bs.b46;last.bs.b54=in.bs.b47;last.bs.b62=in.bs.b48;
	last.bs.b7 =in.bs.b49;last.bs.b15=in.bs.b50;last.bs.b23=in.bs.b51;last.bs.b31=in.bs.b52;last.bs.b39=in.bs.b53;last.bs.b47=in.bs.b54;last.bs.b55=in.bs.b55;last.bs.b63=in.bs.b56;
	last.bs.b8 =in.bs.b57;last.bs.b16=in.bs.b58;last.bs.b24=in.bs.b59;last.bs.b32=in.bs.b60;last.bs.b40=in.bs.b61;last.bs.b48=in.bs.b62;last.bs.b56=in.bs.b63;last.bs.b64=in.bs.b64;
	return last.val;
}
void keymade(unsigned long long K[],unsigned long long Kr[])//编排秘钥
{
	int i;
	for(i=0;i<=Nr;i++){
		Kr[i]=(K[1]<<(4*i))|(K[0]>>(64-4*i));
	}
}
unsigned long long SPN(unsigned long long x,unsigned long long K[])
{//K为128位
	unsigned long long Kr[Nr+1];
	keymade(K,Kr);
	eight_unsigned tb;
	unsigned long long w[Nr],u[Nr+1];
	w[0]=x;
	int r;
	for(r=1;r<Nr;r++){
		u[r]=w[r-1]^Kr[r-1]; //轮秘钥异或
		//对4个部分进行S盒子代换
		tb.val=u[r];
		tb.us.u1=S(tb.us.u1);
		tb.us.u2=S(tb.us.u2);
		tb.us.u3=S(tb.us.u3);
		tb.us.u4=S(tb.us.u4);
		tb.us.u5=S(tb.us.u5);
		tb.us.u6=S(tb.us.u6);
		tb.us.u7=S(tb.us.u7);
		tb.us.u8=S(tb.us.u8);
		//此时tb内存放的就是v[r]
		//进行置换
		w[r]=P(tb);
	}
	u[Nr]=w[Nr-1]^Kr[Nr-1];
	//最后一轮只进行S盒子代替
	tb.val=u[r];
	tb.us.u1=S(tb.us.u1);
	tb.us.u2=S(tb.us.u2);
	tb.us.u3=S(tb.us.u3);
	tb.us.u4=S(tb.us.u4);
	tb.us.u5=S(tb.us.u5);
	tb.us.u6=S(tb.us.u6);
	tb.us.u7=S(tb.us.u7);
	tb.us.u8=S(tb.us.u8);//F4 20 1A 83 89 4C 8F 46
	unsigned long long tem=tb.val^Kr[Nr];
	return tem; //11 11 22 22 33 33 44 44
}
unsigned int SInv(unsigned int in)//代换函数
{
	int i,j,k=0;
	for(i=0;i<16;i++){
		for(j=0;j<16;j++){
			if(SBOX[i][j]==in){
			    k=1;break;
			}
		}
		if(k==1)break;
	}
	unsigned int c;
	c=(i<<4)|j;
	return c;
}
void keymadeInv(unsigned long long K[],unsigned long long Kr[])//编排秘钥
{
	int i;
	eight_unsigned temp;
	for(i=Nr;i>=0;i--){
		Kr[i]=(K[1]<<(64-4*i))|(K[0]>>(4*i));
		if(i>0&&i<Nr){
			temp.val=Kr[i];
			Kr[i]=P(temp);
		}
	}
}
unsigned long long invSPN(unsigned long long x,unsigned long long K[]){
	unsigned long long Kr[Nr+1];
	keymadeInv(K,Kr);
	eight_unsigned tb;
	unsigned long long w[Nr],u[Nr+1];
	w[0]=x;
	int r;
	for(r=1;r<Nr;r++){
		u[r]=w[r-1]^Kr[r-1]; //轮秘钥异或
		//对4个部分进行S盒子代换
		tb.val=u[r];
		tb.us.u1=SInv(tb.us.u1);
		tb.us.u2=SInv(tb.us.u2);
		tb.us.u3=SInv(tb.us.u3);
		tb.us.u4=SInv(tb.us.u4);
		tb.us.u5=SInv(tb.us.u5);
		tb.us.u6=SInv(tb.us.u6);
		tb.us.u7=SInv(tb.us.u7);
		tb.us.u8=SInv(tb.us.u8);
		//此时tb内存放的就是v[r]
		//进行置换
		w[r]=P(tb);
	}
	u[Nr]=w[Nr-1]^Kr[Nr-1];
	//最后一轮只进行S盒子代替
	tb.val=u[r];
	tb.us.u1=SInv(tb.us.u1);
	tb.us.u2=SInv(tb.us.u2);
	tb.us.u3=SInv(tb.us.u3);
	tb.us.u4=SInv(tb.us.u4);
	tb.us.u5=SInv(tb.us.u5);
	tb.us.u6=SInv(tb.us.u6);
	tb.us.u7=SInv(tb.us.u7);
	tb.us.u8=SInv(tb.us.u8);
	return tb.val^Kr[Nr];
}
int b2s(char* str,unsigned long long *bin){
	int i=0;
	*bin=0;
	while(str[i]){
		*bin*=10;
		*bin+=str[i]-'0';
		i++;
	}
	return 0;
}
int hex2dex(char a){
	if(a<='9')return a-'0';
	return a-'a'+10;
}
int b2c(char* str,unsigned char *key){
	int i=0;
	printf("key:");
	while(str[i]){
		
		key[i/2]=(hex2dex(str[i])<<4)|hex2dex(str[i+1]);
		// printf("%.2x",key[i/2]);
		i+=2;
	}
	// printf("\n");
	return 0;
}
int main(int argc,char *argv[]){
	char plain[9];
	char key[16];//={0x09,0x74,0xfe,0x9f,0xac,0x18,0x7a,0xfb,0xc4,0xe1,0x53,0x2a,0x86,0xe7,0x14,0x8d};
	int i;
	unsigned long long enc;//=16847216617508012387;

	if(b2c(argv[1],key)||b2s(argv[2],&enc)){
		printf("wrong param\n");
		return 0;
	}

	// printf("encrypt:%llx\n",SPN(*(unsigned long long *)plain,(unsigned long long *)key));
	((unsigned long long *)plain)[0]=invSPN(enc,(unsigned long long *)key);
	// plain=invSPN(SPN(plain,(unsigned long long *)key),(unsigned long long *)key);
	plain[8]=0;
	printf("%s",(char *)plain);
	return 0;

}