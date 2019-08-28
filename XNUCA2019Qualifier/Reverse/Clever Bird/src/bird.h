#ifndef BIRD_H
#define BIRD_H

#include<Windows.h>
#include<iostream>
#include<stdio.h> 
#include<stdlib.h>
#include<conio.h> 
#include<time.h>
#include<string>
#include<excpt.h>
#include<cstdlib>

using namespace std;

class Birds{
public:
	#define PR_Box printf("¡ö") 
	#define PR_Gold printf("¡ï") 
	#define PR_Ag printf("¡î") 
	#define PR_FBird printf("§À") 
	#define PR_DBird printf("§¶") 
	#define PR_Land printf("©×©×©Ó") 
	#define PR_Bg_TL printf("¨X") 
	#define PR_Bg_TR printf("¨[") 
	#define PR_Bg_DL printf("¨^") 
	#define PR_Bg_DR printf("¨a") 
	#define PR_Bg_X printf("¨T") 
	#define PR_Bg_Y printf("¨U") 
	#define PR_Blank printf(" ")

	COORD bird;
	COORD Bg[5];
	char keyScore[16];
	int pass;
	int Score, Grade, Distance, l_blank, Max_blank;

	Birds();
	void Position(COORD a);
	void Position(int x, int y);
	void PriMenu();
	void init_Bg();
	void Pri_Bg();
	void Pri_Bird();
	bool Check_Bg();
	//void PriGameBar();
	void PriGame();
	void loop_Bg();
	void check_status(char keyScore[]);
	int crccheck();
};

#endif;