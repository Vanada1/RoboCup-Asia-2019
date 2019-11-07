#include "CospaceSettings.hpp"
#include "CommonTools.hpp"
//#include "Hikaru.hpp"
#include "Masuda.hpp"
//#include "Superteam.hpp"
//#include "Test2.hpp"
//#include "AutoStrategy2019.hpp"
//#include "test.hpp"
#include "HikaruForFirstTour.hpp"
#include "TestWorld2AS.hpp"
#include <bits/stdc++.h>

// sprintfのエラーを無視するため
#define _CRT_SECURE_NO_WARNINGS 1

/*
	命名規則名	先頭	区切り	適用先
	PascalCase	大		大		クラス名
	camelCase	小		大		メソッド名、関数名
	snake_case	全て小	_		変数名、メンバ名、ファイル名
	SNAKE_CASE	全て大	_		マクロ名、定数、列挙体
	ただし、Cospaceで既に決まっている変数は上に従わない(Cospaceで定まっているものに同じ形式に変数名を変更する？)
	また、数字などの関係で視認性が悪いときもこれに従わなくて良い
*/
/*
	気をつけること
	・関数の引数がない場合、引数にvoidを入れること。引数がなにもない関数に引数を渡すと、全く別の変数の値が書き換わる。voidを入れると、引数を入れた時点でエラー表示が出る
	・
*/

/*	インクルードについて
ヘッダファイル同士が依存関係を持つことがないようにする
![image](out/include_files/include_files.svg)
*/

using namespace std;

// Game0_Hikaru game0;
Game0_Masuda game0;
//Game0_Superteam game0;
//Game1_Hikaru game1;
Game1_HikaruForFirstTour game1;
//Game1_Test2 game1;
//World2_Test1 game1;
//World2_Test2 game1;
 //Game1_Masuda game1;
//AutoStrategy game1;


//////
int AI_SensorNum = 13;
void movemotors(int, int);
int state = 0;
int z = 1;
int turning = 0;
int superOBX = 0;
int superOBY = 0;
int zerox = 0;									//последние координаты
int zeroy = 0;									//последние координаты
int red = 0, black = 0, cyan = 0;
int count = 0;
int max = 6;									//ограничение LO
int stuck = 0;									//ВРЕМЯ МЕЖДУ ТОЧКАМИ
int escape = 0;
int AI = 0;
int q2 = 1;
int second = 1;
int times = 0;
int turn = 0;
int DepFlag = 0;
int R_L = 0;
int RGB = 0;
int blueflag = 0;
int borderflag = 0;
int magnitflag = 1;
int r;
int stop;
int minSonic;
int localSonic;
int magnitlag = 0;
int flag = 0;
int spr = 55;
int sprobject;

//red definition     		
const int RED_R_B = 255;
const int RED_G_B = 39;
const int RED_B_B = 39;
const int RED_R_s = 231;
const int RED_G_s = 29;
const int RED_B_s = 29;

//black definition
const int BLACK_R_B = 39;
const int BLACK_G_B = 39;
const int BLACK_B_B = 39;
const int BLACK_R_s = 29;
const int BLACK_G_s = 29;
const int BLACK_B_s = 29;

//CYAN definition
const int CYAN_R_B = 39;
const int CYAN_G_B = 255;
const int CYAN_B_B = 39;
const int CYAN_R_s = 29;
const int CYAN_G_s = 249;
const int CYAN_B_s = 29;

//gray definition
const int GRAY_R_B = 117;
const int GRAY_G_B = 124;
const int GRAY_B_B = 159;
const int GRAY_R_s = 102;
const int GRAY_G_s = 109;
const int GRAY_B_s = 144;

//pink definition
const int PINK_R_B = 255;
const int PINK_G_B = 50;
const int PINK_B_B = 255;
const int PINK_R_s = 220;
const int PINK_G_s = 0;
const int PINK_B_s = 230;

//purple definition
const int PURPLE_R_B = 206;
const int PURPLE_G_B = 29;
const int PURPLE_B_B = 199;
const int PURPLE_R_s = 178;
const int PURPLE_G_s = 22;
const int PURPLE_B_s = 178;

//orange definition
//		ORANGE_R_B, ORANGE_G_B,ORANGE_B_B, ORANGE_R_s, ORANGE_G_s, ORANGE_B_s
const int ORANGE_R_B = 255;
const int ORANGE_G_B = 186;
const int ORANGE_B_B = 0;
const int ORANGE_R_s = 204;
const int ORANGE_G_s = 163;
const int ORANGE_B_s = 0;

//yellow definition
const int YELLOW_R_B = 235;
const int YELLOW_G_B = 248;
const int YELLOW_B_B = 0;
const int YELLOW_R_s = 204;
const int YELLOW_G_s = 217;
const int YELLOW_B_s = 0;

//white definition
const int WITE_R_B = 235;
const int WITE_G_B = 248;
const int WITE_B_B = 255;
const int WITE_R_s = 204;
const int WITE_G_s = 217;
const int WITE_B_s = 255;

//blue zone
//      BLUE_R_B, BLUE_G_B, BLUE_B_B, BLUE_R_s, BLUE_G_s, BLUE_B_s
const int BLUE_R_B = 0;
const int BLUE_G_B = 148;
const int BLUE_B_B = 255;
const int BLUE_R_s = 0;
const int BLUE_G_s = 130;
const int BLUE_B_s = 130;

//green zone
const int GREEN_R_B = 0;
const int GREEN_G_B = 171;
const int GREEN_B_B = 100;
const int GREEN_R_s = 0;
const int GREEN_G_s = 150;
const int GREEN_B_s = 90;

//zeroBlue definition 
const int ZEROBLUE_R_B = 0;
const int ZEROBLUE_G_B = 0;
const int ZEROBLUE_B_B = 255;
const int ZEROBLUE_R_s = 0;
const int ZEROBLUE_G_s = 0;
const int ZEROBLUE_B_s = 250;
//////

int triger = 0;

void Game0()
{
	 if (getRepeatedNum() == 0)
	 {
	 	game0.setup();
	 }

	 if (game0.shouldTeleport())
	 {
	 	game0.taskOnTeleport();
	 }
	 else
	 {
	 	game0.loop();
	 }
}

// void Game0()
// {
// 	if (SuperDuration > 0)
// 	{
// 		SuperDuration--;
// 	}

// 	else if (Duration > 0)
// 	{
// 		Duration--;
// 	}

// 	//Телепорт
// 	else if (Time >= 180)
// 	{
// 		red = 0;
// 		cyan = 0;
// 		black = 0;
// 		LoadedObjects = 0;
// 		Teleport = 3;
// 	}

// 	//подобрать красный
// 	else if ((CSLeft_R >= RED_R_s && CSLeft_R <= RED_R_B && CSLeft_G >= RED_G_s && CSLeft_G <= RED_G_B && CSLeft_B >= RED_B_s && CSLeft_B <= RED_B_B || CSRight_R >= RED_R_s && CSRight_R <= RED_R_B && CSRight_G >= RED_G_s && CSRight_G <= RED_G_B && CSRight_B >= RED_B_s && CSRight_B <= RED_B_B) && LoadedObjects < 6 && red < 2)
// 	{
// 		Duration = 43;
// 		red += 1;
// 		CurAction = 1;
// 	} 
// 	//НЕТ ОГРАНИЧЕНИЯ НА КОЛИЧЕСТВО!!!!!!!!!!!!!!!!!!!!!!!!!
// 	//подобрать черный
// 	else if (((CSLeft_R >= BLACK_R_s && CSLeft_R <= BLACK_R_B && CSLeft_G >= BLACK_G_s && CSLeft_G <= BLACK_G_B && CSLeft_B >= BLACK_B_s && CSLeft_B <= BLACK_B_B || CSRight_R >= BLACK_R_s && CSRight_R <= BLACK_R_B && CSRight_G >= BLACK_G_s && CSRight_G <= BLACK_G_B && CSRight_B >= BLACK_B_s && CSRight_B <= BLACK_B_B)) && LoadedObjects < 6 )
// 	{
// 		Duration = 43;
// 		black += 1;
// 		CurAction = 1;
// 	}

// 	//подобрать синий
// 	else if ((CSLeft_R >= CYAN_R_s && CSLeft_R <= CYAN_R_B && CSLeft_G >= CYAN_G_s && CSLeft_G <= CYAN_G_B && CSLeft_B >= CYAN_B_s && CSLeft_B <= CYAN_B_B || CSRight_R >= CYAN_R_s && CSRight_R <= CYAN_R_B && CSRight_G >= CYAN_G_s && CSRight_G <= CYAN_G_B && CSRight_B >= CYAN_B_s && CSRight_B <= CYAN_B_B) && LoadedObjects < 6 && cyan < 2)
// 	{
// 		Duration = 43;
// 		cyan += 1;
// 		CurAction = 1;
// 	}

// 	//депозит Л_П
// 	else if ((CSLeft_R >= ORANGE_R_s && CSLeft_R <= ORANGE_R_B && CSLeft_G >= ORANGE_G_s && CSLeft_G <= ORANGE_G_B && CSLeft_B >= ORANGE_B_s && CSLeft_B <= ORANGE_B_B && CSRight_R >= ORANGE_R_s && CSRight_R <= ORANGE_R_B && CSRight_G >= ORANGE_G_s && CSRight_G <= ORANGE_G_B && CSRight_B >= ORANGE_B_s && CSRight_B <= ORANGE_B_B) && (LoadedObjects >= 4 || (red >= 1 && black >= 1 && cyan >= 1)))
// 	{
// 		Duration = 43;
// 		CurAction = 3;
// 	}

// 	//депозит Л
// 	else if (CSLeft_R >= ORANGE_R_s && CSLeft_R <= ORANGE_R_B && CSLeft_G >= ORANGE_G_s && CSLeft_G <= ORANGE_G_B && CSLeft_B >= ORANGE_B_s && CSLeft_B <= ORANGE_B_B && (LoadedObjects >= 4 || (red >= 1 && black >= 1 && cyan >= 1)))
// 	{
// 		Duration = 0;
// 		CurAction = 4;
// 	}

// 	//депозит_П
// 	else if (CSRight_R >= ORANGE_R_s && CSRight_R <= ORANGE_R_B && CSRight_G >= ORANGE_G_s && CSRight_G <= ORANGE_G_B && CSRight_B >= ORANGE_B_s && CSRight_B <= ORANGE_B_B && (LoadedObjects >= 4 || (red >= 1 && black >= 1 && cyan >= 1)))
// 	{
// 		Duration = 0;
// 		CurAction = 5;
// 	}

// 	//ловушка Л_П
// 	else if (CSLeft_R >= YELLOW_R_s && CSLeft_R <= YELLOW_R_B && CSLeft_G >= YELLOW_G_s && CSLeft_G <= YELLOW_G_B && CSLeft_B >= YELLOW_B_s && CSLeft_B <= YELLOW_B_B && CSRight_R >= YELLOW_R_s && CSRight_R <= YELLOW_R_B && CSRight_G >= YELLOW_G_s && CSRight_G <= YELLOW_G_B && CSRight_B >= YELLOW_B_s && CSRight_B <= YELLOW_B_B && LoadedObjects > 0)
// 	{
// 		Duration = 0;
// 		CurAction = 9;
// 	}

// 	//ловушка Л
// 	else if (CSLeft_R >= YELLOW_R_s && CSLeft_R <= YELLOW_R_B && CSLeft_G >= YELLOW_G_s && CSLeft_G <= YELLOW_G_B && CSLeft_B >= YELLOW_B_s && CSLeft_B <= YELLOW_B_B && LoadedObjects > 0)
// 	{

// 		Duration = 0;
// 		CurAction = 7;
// 	}

// 	//ловушка П
// 	else if (CSRight_R >= YELLOW_R_s && CSRight_R <= YELLOW_R_B && CSRight_G >= YELLOW_G_s && CSRight_G <= YELLOW_G_B && CSRight_B >= YELLOW_B_s && CSRight_B <= YELLOW_B_B && LoadedObjects > 0)
// 	{

// 		Duration = 0;
// 		CurAction = 8;
// 	}
	
// 	/*else if(magnitlag>25)
// 	{
// 		magnitflag=0;
// 	}
		
// 	else if (magnitflag == 1)
// 	{
// 		DepFlag = 5;
// 		Duration = 0;
// 		CurAction = 444;
// 	}*/

// 	//Если прeпятсвие близко спереди
// 	else if (US_Front <= 12)
// 	{
// 		Duration = 0;
// 		CurAction = 12;
// 	}

// 	//Если прeпятсвие спереди
// 	else if (US_Front <= 22)
// 	{
// 		Duration = 0;
// 		CurAction = 13;
// 	}

// 	//Если прeпятсвие слева
// 	else if (US_Left < 17)
// 	{
// 		Duration = 0;
// 		CurAction = 10;
// 	}

// 	//Если прeпятсвие справа
// 	else if (US_Right < 17)
// 	{
// 		Duration = 0;
// 		CurAction = 11;
// 	}
	
	
// 	//Движение
// 	else {
// 		Duration = 0;
// 		CurAction = 2;
// 	}




// 	/////////////////////////////////////////////			КЕЙСЫ		///////////////////////////////////////////////////////////

// 	switch (CurAction)
// 	{//Подбор объекта 
// 	case 1: {

// 		LED_1 = 1;
// 		if (Duration == 1)
// 		{
// 			LoadedObjects++;
// 		}
// 		else if (Duration < 1)
// 		{
// 			movemotors(3, 3);
// 		}
// 		else
// 		{
// 			movemotors(0, 0);
// 		}
// 		break;
// 	}

// 			//Движение		
// 	case 2: {
// 		LED_1 = 0;
// 		R_L = 0;
// 		movemotors(3, 3);

// 		break; }

// 			//Депозит ЛП	
// 	case 3: {
// 		if (Duration == 1)
// 		{
// 			LoadedObjects = 0;
// 			red = 0;
// 			black = 0;
// 			cyan = 0;
// 			RGB = 0;
// 			magnitflag = 1;
// 		}
// 		else if (Duration < 1)
// 		{

// 			movemotors(3, 3);
// 		}
// 		else movemotors(0, 0);
// 		LED_1 = 2;
// 		break; }

// 			//Депозит Л
// 	case 4: {
// 		LED_1 = 0;
// 		movemotors(-1, 3);
// 		break; }

// 			//Деопзит П
// 	case 5: {
// 		LED_1 = 0;
// 		movemotors(3, -1);
// 		break; }

// 			//Ловушка Л		
// 	case 7: {
// 		LED_1 = 0;
// 		magnitflag = 0;
// 		break; }

// 			//Ловушка П		
// 	case 8: {
// 		LED_1 = 0;
// 		magnitflag = 0;
// 		break; }

// 			//Ловушка Л_П		
// 	case 9: {
// 		LED_1 = 0;

// 		magnitflag = 0;
// 		break; }

// 			//Препятствие
// 	case 10: {
// 		LED_1 = 0;
// 		break; }

// 			 //Препятствие
// 	case 11: {
// 		LED_1 = 0;
// 		break; }

// 			 //Препятствие
// 	case 12: {
// 		LED_1 = 0;
// 		break; }

// 			 //Препятствие
// 	case 13: {
// 		LED_1 = 0;
// 		break; }
// 			 //Движение к депозиту
// 	case 14: {
// 		if (magnitflag == 0)
// 		{
// 			magnitflag = 1;
// 		}
// 	}
	
// 	case 444: {
// 		LED_1 = 0;
// 		DepFlag=3;
// 		break;
// 	}
	
// 	}
// }


void Game1()
{
	if (getRepeatedNum() == 0 || triger == 0)
	{
		srand(Time);
		game1.setup();
		triger = 1;
	}
	game1.loop();
}



void movemotors(int x, int y) {
	if (q2 == 1)
	{
		WheelLeft = x;
		WheelRight = y;
	}
	else
	{
		WheelLeft = -y;
		WheelRight = -x;
	}
}
