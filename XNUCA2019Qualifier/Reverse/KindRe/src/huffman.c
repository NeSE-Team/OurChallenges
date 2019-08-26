#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <windows.h>

#define MAX_PATH_LEN 100
#define MAX 65

#define len(x) ((int)log10(x)+1)

/* Node of the huffman tree */
struct node{
    int value;
    char letter;
    struct node *left,*right;
};

typedef struct node Node;

/* 81 = 8.1%, 128 = 12.8% and so on. The 27th frequency is the space. Source is Wikipedia */
unsigned long Frequencies[MAX] = {0};

void getFrequencies()
{
	char filename[MAX_PATH_LEN];//用于存放获取的路径信息。
	GetModuleFileName(NULL,filename,MAX_PATH_LEN);

	FILE * pFile;
    long lSize;
    char * buffer;
    size_t result;
    
    /* 若要一个byte不漏地读入整个文件，只能采用二进制方式打开 */ 
    pFile = fopen (filename, "rb" );
    if (pFile==NULL)
    {
        fputs ("File error",stderr);
        exit (1);
    }
 
    /* 获取文件大小 */
    fseek (pFile , 0 , SEEK_END);
    lSize = ftell (pFile);
    rewind (pFile);
 
    /* 分配内存存储整个文件 */ 
    buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL)
    {
        fputs("Memory error",stderr); 
        exit(2);
    }
 
    /* 将文件拷贝到buffer中 */
    result = fread (buffer,1,lSize,pFile);
    if (result != lSize)
    {
        fputs("Reading error",stderr);
        exit (3);
    }
    /* 现在整个文件已经在buffer中，可由标准输出打印内容 */
    //printf("%s", buffer); 
 
    
	for(int i = 0; i<lSize; i++)
	{
		char ch = buffer[i];
		if(ch>='a' && ch <= 'z')
			Frequencies[ch-'a']++;
		else if(ch>='A' && ch<='Z')
			Frequencies[ch-'A'+26]++;
		else if(ch>='0' && ch<='9')
			Frequencies[ch-'0'+52]++;
		else if(ch=='_')
			Frequencies[62]++;
		else if(ch=='{')
			Frequencies[63]++;
		else if(ch=='}')
			Frequencies[64]++;
	}
	/* 结束演示，关闭文件并释放内存 */
    fclose (pFile);
    free (buffer);
	//for(i = 0; i<65; i++)
		//printf("%d, ", Frequencies[i]);
}
/*finds and returns the small sub-tree in the forrest*/
int findSmaller (Node *array[], int differentFrom){
    int smaller;
    int i = 0;

    while (array[i]->value==-1)
        i++;
    smaller=i;
    if (i==differentFrom){
        i++;
        while (array[i]->value==-1)
            i++;
        smaller=i;
    }

    for (i=1;i<MAX;i++){
        if (array[i]->value==-1)
            continue;
        if (i==differentFrom)
            continue;
        if (array[i]->value<array[smaller]->value)
            smaller = i;
    }

    return smaller;
}

/*builds the huffman tree and returns its address by reference*/
void buildHuffmanTree(Node **tree){
    Node *temp;
    Node *array[MAX];
    int i, subTrees = MAX;
    int smallOne,smallTwo;

    for (i=0;i<MAX;i++){
        array[i] = (Node *)malloc(sizeof(Node));
        array[i]->value = Frequencies[i];
        array[i]->letter = i;
        array[i]->left = NULL;
        array[i]->right = NULL;
    }

    while (subTrees>1){
        smallOne=findSmaller(array,-1);
        smallTwo=findSmaller(array,smallOne);
        temp = array[smallOne];
        array[smallOne] = (Node *)malloc(sizeof(Node));
        array[smallOne]->value=temp->value+array[smallTwo]->value;
        array[smallOne]->letter=127;
        array[smallOne]->left=array[smallTwo];
        array[smallOne]->right=temp;
        array[smallTwo]->value=-1;
        subTrees--;
    }

    *tree = array[smallOne];

	return;
}

/* builds the table with the bits for each letter. 1 stands for binary 0 and 2 for binary 1 (used to facilitate arithmetic)*/
void fillTable(int codeTable[], Node *tree, int Code){
    if (tree->letter<MAX)
        codeTable[(int)tree->letter] = Code;
    else{
        fillTable(codeTable, tree->left, Code*10+1);
        fillTable(codeTable, tree->right, Code*10+2);
    }

    return;
}

/*function to compress the input*/
void compressFile(FILE *input, FILE *output, int codeTable[]){
    char bit, ch, x = 0;
    int n,length,bitsLeft = 8;
    int originalBits = 0, compressedBits = 0;
	char flag[60];
	fscanf(input,"%40s", flag);
	//printf("%s\n", flag);
	if(strlen(flag)!=40) exit(0);

    fclose(input);

    for(int i = 0; i<40; i++)
	{
		ch = flag[i];
        originalBits++;
		length = 0;
		if(ch>='a' && ch <= 'z'){
            length = len(codeTable[ch-'a']);
            n = codeTable[ch-'a'];
        }
		else if(ch>='A' && ch<='Z'){
            length = len(codeTable[ch-'A'+26]);
            n = codeTable[ch-'A'+26];
        }
		else if(ch>='0' && ch<='9'){
            length = len(codeTable[ch-'0'+52]);
            n = codeTable[ch-'0'+52];
        }
		else if(ch=='_'){
            length = len(codeTable[62]);
            n = codeTable[62];
        }
		else if(ch=='{'){
            length = len(codeTable[63]);
            n = codeTable[63];
        }
		else if(ch=='}'){
            length = len(codeTable[64]);
            n = codeTable[64];
        }

        while (length>0){
            compressedBits++;
            bit = n % 10 - 1;  //还是01编码
            n /= 10;
            x = x | bit;
            bitsLeft--;
            length--;
            if (bitsLeft==0){
                fputc(x,output);
                x = 0;
                bitsLeft = 8;
            }
            x = x << 1;
        }
    }

    if (bitsLeft!=8){
        x = x << (bitsLeft-1);  //用0补足
        fputc(x,output);
    }

    /*print details of compression on the screen*/
    //fprintf(stderr,"Original bits = %dn",originalBits*8);
    //fprintf(stderr,"Compressed bits = %dn",compressedBits);
    //fprintf(stderr,"Saved %.2f%% of memoryn",((float)compressedBits/(originalBits*8))*100);

    return;
}

/*function to decompress the input*/

void decompressFile (FILE *input, FILE *output, Node *tree){
    Node *current = tree;
    char c, ch, bit;
    char mask = (char)1 << 7;
    int i;

    long lSize;
    char * buffer;
    size_t result;
 
    /* 获取文件大小 */
    fseek (input , 0 , SEEK_END);
    lSize = ftell (input);
    rewind (input);
 
    /* 分配内存存储整个文件 */ 
    buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL)
    {
        fputs("Memory error",stderr); 
        exit(2);
    }
 
    /* 将文件拷贝到buffer中 */
    result = fread (buffer,1,lSize,input);
    if (result != lSize)
    {
        fputs("Reading error",stderr);
        exit (3);
    }

    fclose(input);
    for(int j = 0; j<lSize; j++)
	{
		c = buffer[j];
        for (i=0;i<8;i++){
            bit = c & mask;
            c = c << 1;
            if (bit==0){
                current = current->left;
				ch = current->letter;
                if(ch!=127){
					if(ch>=0 && ch <= 25){
						fputc(ch+'a', output);
					}
					else if(ch>=26 && ch<=51){
						fputc(ch-26+'A', output);
					}
					else if(ch>=52 && ch<=61){
						fputc(ch-52+'0', output);
					}
					else if(ch==62){
						fputc('_', output);
					}
					else if(ch==63){
						fputc('{', output);
					}
					else if(ch==64){
						fputc('}', output);
					}
                    current = tree;
                }
            }

            else{
                current = current->right;
				ch = current->letter;
               if(ch!=127){
					if(ch>=0 && ch <= 25){
						fputc(ch+'a', output);
					}
					else if(ch>=26 && ch<=51){
						fputc(ch-26+'A', output);
					}
					else if(ch>=52 && ch<=61){
						fputc(ch-52+'0', output);
					}
					else if(ch==62){
						fputc('_', output);
					}
					else if(ch==63){
						fputc('{', output);
					}
					else if(ch==64){
						fputc('}', output);
					}
                    current = tree;
                }
            }
        }
    }

    return;
}

/*invert the codes in codeTable2 so they can be used with mod operator by compressFile function*/
void invertCodes(int codeTable[],int codeTable2[]){
    int i, n, copy;

    for (i=0;i<MAX;i++){
        n = codeTable[i];
        copy = 0;
        while (n>0){
            copy = copy * 10 + n %10;
            n /= 10;
        }
        codeTable2[i]=copy;
    }

	return;
}

int main(){
    Node *tree;
    int codeTable[MAX], codeTable2[MAX];
    int compress;
    char filename[20];
	char flag[60];
    FILE *input, *output;
	getFrequencies();
	
	buildHuffmanTree(&tree);

    fillTable(codeTable, tree, 0);

    invertCodes(codeTable,codeTable2);
	
	input = fopen("flag.txt", "r");
	if(input == NULL)
    {
		printf("Open file failure!");
		exit(1);
    }
    else
    {
		output = fopen("output.txt","w");
		compressFile(input,output,codeTable2);
		fclose(output);
    }
	/*
	input = fopen("output.txt", "rb");
	if(input == NULL)
    {
		printf("Open file failure!");
		exit(1);
    }
    else
    {
		output = fopen("output1.txt","w");
		decompressFile(input,output, tree);
		fclose(output);
    }*/
	
    
	
/*
	char filename[MAX_PATH];//用于存放获取的路径信息。
	GetModuleFileName(NULL,filename,MAX_PATH);//

    input = fopen(filename, "r");
    output = fopen("output.txt","w");

    //if (compress==1)
        compressFile(input,output,codeTable2);
    //else
    //    decompressFile(input,output, tree);
*/
    return 0;
}
