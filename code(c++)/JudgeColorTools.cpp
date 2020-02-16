#include "JudgeColorTools.hpp"

// If SuperObj, you will be a plus
int color_width = 10;
int super_obj_color_width = color_width + 20;

int object_box[3][2] = {{204, 235}, {130, 148}, {0, 0}};
//int object_box2[3][2] = {{204, 235}, {130, 148}, {0, 0}};
int object_box2[3][2] = {{204, 235}, {150, 171}, {111, 123}};
//int object_box2[3][2] = { {189, 217}, {106, 121}, {55, 61} };
//int object_box2[3][2] = { {204, 235}, {109, 124}, {0, 0} };
//int object_box2[3][2] = {{204, 235}, {163, 186}, {0, 0}};
//int object_box[3][2] = { {189, 217}, {106, 121}, {55, 61} };
//int object_box[3][2] = { {204, 235}, {109, 124}, {0, 0} };
//int object_box[3][2] = {{204, 235}, {163, 186}, {0, 0}};
// int gray_zone[3][2] = {{130, 160}, {140, 165}, {185, 210}};
//int gray_zone[3][2] = {{133, 153}, {141, 161}, {187, 207}};
// int gray_zone[3][2] = {{125, 141}, {132, 150}, {169, 198}};
//int gray_zone[3][2] = {{165, 190}, {176, 201}, {233, 255}};
// int gray_zone[3][2] = {{116, 102}, {123, 108}, {158, 143}};
int sp_obj[3][2] = {{200, 255}, {0, 41}, {200, 255}};
int purple_line[3][2] = {{150, 180}, {80, 100}, {180, 220}};
int white_zone[3][2] = {{204, 235}, {217, 248}, {255, 255}};
int world1_maker[3][2] = {{188, 216}, {58, 66}, {230, 254}};
//int blue_trap[3][2] = {{54, 62}, {97, 111}, {221, 245}};
int blue_trap[3][2] = {{0, 0}, {95, 108}, {217, 240}};

int last_color_sow[3][2] = {{0,0}, {0,0}, {0,0}};

int red_obj[3][2] = {{232, 255}, {29, 39}, {29, 39}};
//int cyan_obj[3][2] = {{29, 39}, {249, 255}, {29, 39}};
int cyan_obj[3][2] = {{29, 39}, {249, 255}, {249, 255}};
int black_obj[3][2] = {{29, 39}, {29, 39}, {29, 39}};
// int trap_line[3][2] = {{200, 235}, {215, 250}, {0, 0}};
int trap_line[3][2] = {{204, 235}, {217, 248}, {0, 0}};
int trap_line2[3][2] = {{167, 192}, {178, 203}, {43, 48}};
int blue_zone[3][2] = {{0, 0}, {150, 175}, {255, 255}};

int gray_zone[3][2] = {{141, 162}, {150, 171}, {198, 219}};


int ColorJudgeLeft(int col[3][2])
{
	return (col[0][0] - color_width <= CSLeft_R && CSLeft_R <= col[0][0] + color_width 
			&& col[1][0] - color_width <= CSLeft_G && CSLeft_G <= col[1][0] + color_width 
			&& col[2][0] - color_width <= CSLeft_B && CSLeft_B <= col[2][0] + color_width) 
			|| (col[0][1] - color_width <= CSLeft_R && CSLeft_R <= col[0][1] + color_width 
				&& col[1][1] - color_width <= CSLeft_G && CSLeft_G <= col[1][1] + color_width 
				&& col[2][1] - color_width <= CSLeft_B && CSLeft_B <= col[2][1] + color_width);
}
int ColorJudgeRight(int col[3][2])
{
	return (col[0][0] - color_width <= CSRight_R && CSRight_R <= col[0][0] + color_width && col[1][0] - color_width <= CSRight_G && CSRight_G <= col[1][0] + color_width && col[2][0] - color_width <= CSRight_B && CSRight_B <= col[2][0] + color_width) ||
		   (col[0][1] - color_width <= CSRight_R && CSRight_R <= col[0][1] + color_width && col[1][1] - color_width <= CSRight_G && CSRight_G <= col[1][1] + color_width && col[2][1] - color_width <= CSRight_B && CSRight_B <= col[2][1] + color_width);
}
int SameColorCheck(int color1[3][2], int color2[3][2])
{
	return (color1[0][0] == color2[0][0] && color1[1][0] == color2[1][0] && color1[2][0] == color2[2][0] ) &&
		   (color1[0][1] == color2[0][1] && color1[1][1] == color2[1][1] && color1[2][1] == color2[2][1] );

}
int EitherColorJudge(int col[3][2])
{
	return ColorJudgeLeft(col) || ColorJudgeRight(col);
}
int BothColorJudge(int col[3][2])
{
	return ColorJudgeLeft(col) && ColorJudgeRight(col);
}

#define COLOR_TYPE_NUMBER 14
#define COLOR_YELLOW 0
#define COLOR_YELLOW2 12
#define LAST_COLOR_SOW 13
#define COLOR_RED 1
#define COLOR_CYAN 2
#define COLOR_BLACK 3
#define COLOR_SUPER_OBJ 4
#define COLOR_BLUE_FLOOR 5
#define COLOR_DEPOSIT 6
#define COLOR_SWAMPLAND 7
#define COLOR_WHITE 8
#define COLOR_MAKER1 9
#define COLOR_DEPOSIT2 10
#define COLOR_BLUE_TRAP 11
int color_world2[COLOR_TYPE_NUMBER][6];
int ColorInformationInputer(int num, int col[3][2])
{
	logErrorMessage.logMessage("ColorInformationInputer() : Input new color", MODE_VERBOSE);
	if (num >= COLOR_TYPE_NUMBER)
	{
		logErrorMessage.errorMessage("ColorInformationInputer(): num's value is strange\n", MODE_NORMAL);
		return 0;
	}
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 2; j++)
		{
			if (col[i][j] < 0 || col[i][j] > 255)
			{
				logErrorMessage.errorMessage("ColorInformationInputer() : value > 255", MODE_NORMAL);
			}
			color_world2[num][j * 3 + i] = col[i][j];
		}
	}
	logErrorMessage.logMessage("ColorInformationInputer() successful\n", MODE_VERBOSE);
	return 1;
}
void InputColorInformation(void)
{
	logErrorMessage.logMessage("Start InputColorInformation()\n", MODE_VERBOSE);
	ColorInformationInputer(COLOR_YELLOW, trap_line);
	ColorInformationInputer(COLOR_YELLOW2, trap_line2);
	ColorInformationInputer(COLOR_RED, red_obj);
	ColorInformationInputer(COLOR_CYAN, cyan_obj);
	ColorInformationInputer(COLOR_BLACK, black_obj);
	ColorInformationInputer(COLOR_SUPER_OBJ, sp_obj);
	ColorInformationInputer(COLOR_BLUE_FLOOR, blue_zone);
	ColorInformationInputer(COLOR_DEPOSIT, object_box);
	ColorInformationInputer(COLOR_SWAMPLAND, gray_zone);
	ColorInformationInputer(COLOR_WHITE, white_zone);
	ColorInformationInputer(COLOR_MAKER1, world1_maker);
	ColorInformationInputer(COLOR_DEPOSIT2, object_box2);
	ColorInformationInputer(COLOR_BLUE_TRAP, blue_trap);
	logErrorMessage.logMessage("End InputColorInformation\n", MODE_VERBOSE);
}

int IsOnStuff(int num)
{
	int ans = 0;
	int local_color_width = color_width;
	if (num == COLOR_SUPER_OBJ)
	{
		local_color_width = super_obj_color_width;
	}
	if ((color_world2[num][0] + local_color_width > CSLeft_R && color_world2[num][0] - local_color_width < CSLeft_R && color_world2[num][1] + local_color_width > CSLeft_G && color_world2[num][1] - local_color_width < CSLeft_G && color_world2[num][2] + local_color_width > CSLeft_B && color_world2[num][2] - local_color_width < CSLeft_B) || 
		(color_world2[num][3] + local_color_width > CSLeft_R && color_world2[num][3] - local_color_width < CSLeft_R && color_world2[num][4] + local_color_width > CSLeft_G && color_world2[num][4] - local_color_width < CSLeft_G && color_world2[num][5] + local_color_width > CSLeft_B && color_world2[num][5] - local_color_width < CSLeft_B))
	{
		ans++;
	}
	if ((color_world2[num][0] + local_color_width > CSRight_R && color_world2[num][0] - local_color_width < CSRight_R && color_world2[num][1] + local_color_width > CSRight_G && color_world2[num][1] - local_color_width < CSRight_G && color_world2[num][2] + local_color_width > CSRight_B && color_world2[num][2] - local_color_width < CSRight_B) ||
		(color_world2[num][3] + local_color_width > CSRight_R && color_world2[num][3] - local_color_width < CSRight_R && color_world2[num][4] + local_color_width > CSRight_G && color_world2[num][4] - local_color_width < CSRight_G && color_world2[num][5] + local_color_width > CSRight_B && color_world2[num][5] - local_color_width < CSRight_B))
	{
		ans += 2;
	}
	return ans;
}
int IsOnYellowLine(void)
{
	return IsOnStuff(COLOR_YELLOW);
}
int IsOnYellowLine2(void)
{
	return IsOnStuff(COLOR_YELLOW2);
}
int IsOnRedObj(void)
{
	return IsOnStuff(COLOR_RED);
}
int IsOnCyanObj(void)
{
	return IsOnStuff(COLOR_CYAN);
}
int IsOnBlackObj(void)
{
	return IsOnStuff(COLOR_BLACK);
}
int IsOnSuperObj(void)
{
	return IsOnStuff(COLOR_SUPER_OBJ);
}
int IsOnBlueFloor(void)
{
	return IsOnStuff(COLOR_BLUE_FLOOR);
}
int IsOnDepositArea(void)
{
	//if (CurGame == 0) {

		return IsOnStuff(COLOR_DEPOSIT2);
	/*}
	else {
		return IsOnStuff(COLOR_DEPOSIT2);
	}*/
}
int IsOnSwampland(void)
{
	return IsOnStuff(COLOR_SWAMPLAND);
}
int IsOnWhiteArea(void)
{
	return IsOnStuff(COLOR_WHITE);
}
int IsOnWorld1MakerArea(void)
{
	return IsOnStuff(COLOR_MAKER1);
}
int IsOnTrapBlue(void)
{
	return IsOnStuff(COLOR_BLUE_TRAP);
}