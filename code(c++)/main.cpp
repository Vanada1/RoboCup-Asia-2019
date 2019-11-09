#include "CospaceSettings.hpp"
#include "CommonTools.hpp"
#include "Hikaru.hpp"
#include "Masuda.hpp"
//#include "Superteam.hpp"
//#include "Test2.hpp"
//#include "AutoStrategy2019.hpp"
//#include "test.hpp"
//#include "HikaruForFirstTour.hpp"
#include "TestWorld2AS.hpp"
#include <bits/stdc++.h>

#define _CRT_SECURE_NO_WARNINGS 1

using namespace std;

// Game0_Hikaru game0;
Game0_Masuda game0;
//Game0_Superteam game0;
Game1_Hikaru game1;
//Game1_HikaruForFirstTour game1;
//Game1_Test2 game1;
//World2_Test1 game1;
//World2_Test2 game1;
 //Game1_Masuda game1;
//AutoStrategy game1;

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
