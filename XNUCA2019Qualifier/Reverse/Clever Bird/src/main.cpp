#include"bird.h"

void HideCursor(){
	CONSOLE_CURSOR_INFO cursor_info = {1 , false};
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), & cursor_info);
}

int getvalue1(int *check) {
	DWORD fileSize, NumberOfBytesRW;
	TCHAR szFileName[MAX_PATH];
	TCHAR  *pBuffer = NULL;
	GetModuleFileName(NULL, szFileName, MAX_PATH);
	HANDLE hFile = CreateFile(
		szFileName,
		GENERIC_READ,
		FILE_SHARE_READ,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		fileSize = GetFileSize(hFile, NULL);
		if (fileSize == 0xFFFFFFFF) return false;
		pBuffer = new TCHAR[fileSize];
		ReadFile(hFile, pBuffer, fileSize, &NumberOfBytesRW, NULL);
		CloseHandle(hFile);
	}
	check[0] = pBuffer[fileSize - 4] & 0xff;
	check[1] = pBuffer[fileSize - 3] & 0xff;
	check[2] = pBuffer[fileSize - 2] & 0xff;
	check[3] = pBuffer[fileSize - 1] & 0xff;
	return true;
}


int main(){
	HideCursor();
	Birds Bird;
	Bird.PriMenu();
	Bird.pass = Bird.crccheck();
	Bird.PriMenu();
	system("cls");
	Bird.PriGame();
	while(1){
		if(Bird.Check_Bg()) 
			break;
		Bird.loop_Bg();
		Bird.Pri_Bg();
		Bird.bird.Y += 1;
		Bird.Position(Bird.bird.X, Bird.bird.Y-1); 
		printf(" ");
		if(GetAsyncKeyState(VK_UP)) 
		{
			Bird.bird.Y -= 4;			
		}
		Bird.Pri_Bird();
		Sleep(150);
	}
	Bird.Position(38,10); 

	//check1 
	if (Bird.pass != Bird.crccheck())
		return 1;

	//score 1066 to get value 3060
	long i;
	float number = (float)Bird.Score;
	float  x2, y;
	//END TEST
	//number = (float)131074; 
	const float threehalfs = 1.5f;
	x2 = number * 0.5f;
	y = number;
	i = *(long *)&y;
	i = 1597463000 - (i >> 1);
	if (i != 0) {
		i = i + 7;
	}
	//check2 
	if ((Bird.pass != Bird.crccheck()) && (Bird.Score != 0))
		return 1;

	y = *(float *)&i;
	y = y * (threehalfs - (x2*y*y));

	int value = (int)(((y * 100000000) * 10 + 5) / 10);

	//check2 
	if ((Bird.Score > 0) && (Bird.pass != Bird.crccheck()))
		return 1;

	if (value == 276142) {
		cout<<"Your Door, pls key:";
		//new stage with score
		//memory realloc
		Sleep(150);
		string key;
		cin >> key;
		char keys[38];
		strcpy_s(keys, key.c_str());
		
		//new window, score to binary
		system("cls");
		int endscore = Bird.Score;
		int counter = 4;
		while (endscore != 0) {
			if (keys[counter++] != '0' + (endscore % 2)) {
				cout << "wrong" << endl;
				return 1;
			}
			endscore = endscore / 2;
		}

		//char transform
		//first four char 
		int i;
		int keyi[4];
		int check[4] = {0};
		getvalue1(check);
		for (i = 0; i < 4; i++) {
			//
			keyi[i] = keys[i] - '0';
			keyi[i] = keyi[i] ^ ((Bird.pass >> (i * 8)) & 0xff);
			if (check[i] != keyi[i]) {
				cout << "wrong" << endl;
				return 1;
			}
		}
		cout << "congratulation,submit with format: flag{key}" << endl;
	}
	else {
		cout<<"You lost."<<endl;
	}
	system("pause");
	return 0;
}

