#include"bird.h"

Birds::Birds(){
	Score = 0;
	Grade = 1;
	Distance = 18;
	Max_blank = 9;
	l_blank = Max_blank - Grade;

}

void Birds::Position(COORD a){
	HANDLE Out = GetStdHandle(STD_OUTPUT_HANDLE); 
	SetConsoleCursorPosition(Out, a);
}

void Birds::Position(int x, int y){
	COORD pos = {x, y}; 
	Position(pos);
}

void Birds::PriMenu(){
	int i; 
	Position(1, 1);
	PR_Bg_TL; 
	Position(77, 1);
	PR_Bg_TR; 
	Position(1, 24);
	PR_Bg_DL; 
	Position(77, 24);
	PR_Bg_DR;
	for(i = 3; i <= 76; i += 2) 
	{ 
		Position(i, 1);
		PR_Bg_X; 
		Position(i, 24);
		PR_Bg_X; 
	} 
	for(i = 2; i <= 23; i++) 
	{
		Position(1,i);
		PR_Bg_Y; 
		Position(77,i);
		PR_Bg_Y; 
	}
	Position(33, 10);
	printf("Flappy Bird");
	Position(33, 12);	
	printf("YOU KNOW:"); 
	Position(33, 13);
	PR_Gold;
	printf("UP FLY");
	Position(33, 14);
	PR_Gold;
	printf("MORE LEVEL, MORE HARD");
	Position(1, 25);
	system("pause");
}

void Birds::init_Bg(){
	int temp;
	for(int i = 0; i < 5; i++){
		Bg[i].X = 74 + i * Distance;
		Bg[i].Y = 10;
		//srand((int)time(0)); 
		temp = rand();
		if(temp % 2 == 0) 
			{ 
				if((temp % 4 + Bg[i].Y + Max_blank - Grade) <  21) 
					Bg[i].Y = Bg[i].Y + temp % 4; 
				else
					Bg[i].Y = Bg[i].Y;  
			} 
		else
		{ 
			if((Bg[i].Y - temp % 4) > 2) 
				Bg[i].Y = Bg[i].Y - temp % 4; 
			else
				Bg[i].Y = Bg[i].Y; 
		}
		//Sleep(1000);
	}
}

void Birds::Pri_Bg(){ 
	int i = 0, k, j, temp, a = 4; 
	l_blank = Max_blank - Grade;
	for(i; i<5; i++)
	{ 
		if(Bg[i].X > 0 && Bg[i].X <= 74) 
		{ 
			for(k = 1; k < Bg[i].Y; k++) 
			{
				Position(Bg[i].X + 1, k); 
				PR_Box;
				PR_Box;
				PR_Blank;
			} 
			Position(Bg[i]); 
			PR_Box;
			PR_Box;
			PR_Box;
			PR_Blank; 
			Position(Bg[i].X, Bg[i].Y + l_blank); 
			PR_Box;
			PR_Box;
			PR_Box;
			PR_Blank; 
			k = Bg[i].Y + l_blank + 1; 
			for(k; k <= 22; k++) 
			{
				Position(Bg[i].X + 1, k); 
				PR_Box;
				PR_Box;
				PR_Blank; 
			} 
			for(j = 0; j < 23; j++){
				Position(1, j);
				PR_Blank;
				PR_Blank;
				PR_Blank;
				PR_Blank; 
				PR_Blank;
			}
		}
		
		if(Bg[i].X == 0) 
		{ 
			if(i == 0)
				Bg[i].X = Bg[4].X + Distance;
			else
				Bg[i].X = Bg[i - 1].X + Distance;
			srand((int)time(0)); 
			temp=rand();
			if(temp % 2 == 0) 
				{ 
					if((temp % 4 + Bg[i].Y + Max_blank - Grade) <  21) 
						Bg[i].Y = Bg[i].Y + temp % 4; 
					else
						Bg[i].Y = Bg[i].Y;  
				} 
			else
			{ 
				if((Bg[i].Y - temp % 4) > 2) 
					Bg[i].Y = Bg[i].Y - temp % 4; 
				else
					Bg[i].Y = Bg[i].Y; 
			}
		}
	}
}

bool Birds::Check_Bg(){
	for(int i = 0; i < 5; i++) 
	{ 
		if(bird.Y > 23) 
			return true; 
		if((bird.X == Bg[i].X || bird.X == Bg[i].X + 5) && bird.Y == Bg[i].Y ) 
			return true; 
		if((bird.X == Bg[i].X+1 || bird.X == Bg[i].X + 2 || bird.X == Bg[i].X + 3 || bird.X == Bg[i].X+4) && bird.Y <= Bg[i].Y) 
			return true; 
		if((bird.X == Bg[i].X || bird.X == Bg[i].X + 5) && bird.Y == Bg[i].Y + l_blank) 
			return true; 
		if((bird.X == Bg[i].X+1 || bird.X == Bg[i].X + 2 || bird.X == Bg[i].X + 3 || bird.X == Bg[i].X+4) && bird.Y >= Bg[i].Y + l_blank) 
			return true;  
	} 
	return false;
}

void Birds::Pri_Bird(){
	Position(bird);
	PR_FBird;
}

void Birds::PriGame(){
	Position(38 ,0); 
	printf("Score:%d", Score);
	Position(1, 23);
	printf("�ששөששөששөששөששөששөששөששөששөששөששөששөשө�");
	bird.X = 41;
	bird.Y = 10;
	Pri_Bird();
	init_Bg();
	Pri_Bg();
}


void Birds::loop_Bg(){
	int level = 20;
	for(int i = 0; i < 5; i++) 
	{
		Bg[i].X -= 1;  
		if(bird.X == Bg[i].X) 
		{
			Score += 1; 
			if (Score % level == 0 && Grade < 4) {
				Grade++;
				level = level * 4;
			}
				 
		} 

	} 
	Position(38 ,0); 
	printf("Score:%d", Score);
}

int Birds::crccheck() {
	//check detect
	int result;
	__try
	{
		__asm int 3;
		//NOT OK;
		result = rand();
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		DWORD fileSize, NumberOfBytesRW;
		int CRCVALUE[4] = { 0,0,0,0 };
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
		DWORD i;
		for (i = 0; i < (fileSize - 4); i++) {
			int stage = i / ((fileSize / 4));
			CRCVALUE[stage] = CRCVALUE[stage] ^ pBuffer[i] & 0xff;
		}
		result = ((CRCVALUE[0] << 24) & 0xff000000) + ((CRCVALUE[1] << 16) & 0xff0000) + ((CRCVALUE[2] << 8) & 0xff00) + (CRCVALUE[3] & 0xff);
	}
	return result;
}