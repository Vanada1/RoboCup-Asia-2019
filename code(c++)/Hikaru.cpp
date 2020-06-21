/*
todo : Addressed an issue where color sensors would enter swamps when rotating
Odometry solves the problem of trapping in PLA
360 x 270
*/
#include "Hikaru.hpp"
#include "MapData.hpp"
#include <iostream>

#define IF if
#define LOG_MESSAGE(MESSAGE, OPTION) \
	IF((OPTION) <= getRunMode()) { logErrorMessage.logMessage((MESSAGE), (OPTION)); }
#define ERROR_MESSAGE(MESSAGE, OPTION) \
	IF((OPTION) <= getRunMode()) { logErrorMessage.errorMessage((MESSAGE), (OPTION)); }

#define POINT_BLACK 20
#define POINT_RED 10
#define POINT_CYAN 15
#define POINT_UNKNOWN 3
#define POINT_SUPERAREA 2
#define POINT_WHITE 1
#define POINT_DEPOSIT 0
#define POINT_SWAMPLAND -1
#define POINT_WALL -2
#define POINT_YELLOW -3
#define POINT_MAY_SWAMPLAND -4

#define SECURE_YELLOW 0
#define SECURE_SWAMPLAND 1
#define SECURE_WALL 3
#define SECURE_WHITE 4
#define SECURE_DEPOSIT 5
#define SECURE_SUPERAREA 6

#define LEFT 1
#define RIGHT 2

#define FUNC_NAME getFuncName(__FUNCTION__)

using namespace std;
void Game1_Hikaru::setup(void)
{
	srand(Time);
	system("cls");
	UserGame1::setup();
	InputDotInformation();
	InputColorInformation();
	resetLoadedObjects();
	logErrorMessage.delLogFile();
	logErrorMessage.delErrorFile();
	system("chcp 65001");

	if (PositionX == 0 && PositionY == 0)
	{

		int y = emergency_now_dot_id / kDotWidthNum;
		int x = emergency_now_dot_id - kDotWidthNum * y;
		dot_x[1] = x;
		dot_y[1] = y;
		log_y = y * kSize;
		log_x = x * kSize;
		now_dot_id = emergency_now_dot_id;
	}
	else
	{
		log_x = PositionX;
		log_y = PositionY;
	}

	rep(i, static_cast<int>(extent<decltype(next_allowed_go_time), 0>::value)) {
		rep(j, static_cast<int>(extent<decltype(next_allowed_go_time), 1>::value)) {
			next_allowed_go_time[i][j] = 0;
		}
	}

	setAction(DEFINED);
	Duration = 0;
	SuperDuration = 0;
	LED_1 = 0;

	setRunMode(MODE_MATCH);
}

void Game1_Hikaru::loop()
{
	ProcessingTime pt;
	pt.start();

	UserGame1::loop();

	static int same_time = 0;
	static int prev_repeated_num = 0;
	if (PositionX != 0 || PositionY != 0)
	{
		log_x = PositionX;
		log_y = PositionY;

		if (log_x < 0)
		{
			log_x = 0;
		}
		if (log_x >= kCospaceWidth)
		{
			log_x = kCospaceWidth - 1;
		}
		if (log_y < 0)
		{
			log_y = 0;
		}
		if (log_y >= kCospaceHeight)
		{
			log_y = kCospaceHeight - 1;
		}
		CheckNowDot();

		int range = 0;
		rep(hi, range * 2 + 1)
		{
			rep(wj, range * 2 + 1)
			{
				int y = hi + dot_y[1] - range;
				int x = wj + dot_x[1] - range;
				if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
				{
					continue;
				}
				dot[y * kDotWidthNum + x].arrived_times++;
			}
		}
	}
	else
	{
		PositionX = -1;
		int range = 2;
		for (int wi = 0; wi < range * 2; wi++)
		{
			for (int hj = 0; hj < range * 2; hj++)
			{
				int x = dot_x[1] + wi - range;
				int y = dot_y[1] + hj - range;
				if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
				{
					continue;
				}
				dot[y * kDotWidthNum + x].arrived_times += 10;
			}
		}
	}

	if (SuperObj_Num != 0)
	{
		if (log_superobj_num <= 0 && (log_superobj_x[0] != SuperObj_X || log_superobj_y[0] != SuperObj_Y))
		{
			//I separate this from *1 because log_superobj_x[0 - 1] makes error
			log_superobj_x[0] = SuperObj_X;
			log_superobj_y[0] = SuperObj_Y;
			log_superobj_num++;
		}
		else
		{
			//*1
			//This avoid record same data many times
			// if the new SuperObject is undefined
			// hint : if log_superobj_num == 0, this doesn't work. so I separate this case
			if (log_superobj_x[log_superobj_num - 1] != SuperObj_X || log_superobj_y[log_superobj_num - 1] != SuperObj_Y)
			{
				log_superobj_x[log_superobj_num] = SuperObj_X;
				log_superobj_y[log_superobj_num] = SuperObj_Y;
				log_superobj_num++;
			}
		}
	}

	calculateWallPosition();
	saveColorInfo();
	//InputDotInformation();
	//InputColorInformation();

	if (SuperDuration > 0)
	{
		SuperDuration--;
	}
	else if (IsOnTrapBlue() != 0)
	{
		resetLoadedObjects();
	}
	else if (IsOnRedObj() && LoadedObjects < 6 && (loaded_objects[RED_LOADED_ID] < kBorderSameObjNum || Time > 450) && !(LoadedObjects == 5 && log_superobj_num >= 1))
	{
		setAction(FIND_OBJ);
		loaded_objects[RED_LOADED_ID]++;
		SuperDuration = kFindObjDuration;
	}
	else if (IsOnCyanObj() && LoadedObjects < 6 && (loaded_objects[CYAN_LOADED_ID] < kBorderSameObjNum || Time > 450) && !(LoadedObjects == 5 && log_superobj_num >= 1))
	{
		setAction(FIND_OBJ);
		loaded_objects[CYAN_LOADED_ID]++;
		SuperDuration = kFindObjDuration;
	}
	else if (IsOnBlackObj() && LoadedObjects < 6 && (loaded_objects[BLACK_LOADED_ID] < kBorderSameObjNum || Time > 450) && !(LoadedObjects == 5 && log_superobj_num >= 1))
	{
		setAction(FIND_OBJ);
		loaded_objects[BLACK_LOADED_ID]++;
		SuperDuration = kFindObjDuration;
	}
	else if (IsOnSuperObj() && SuperObj_Num == 0 && !(IsOnRedObj() || IsOnBlackObj() || IsOnCyanObj()))
	{
		same_time = 0;
		setAction(FIND_OBJ);
		SuperDuration = kFindObjDuration;
		int min = 10000;
		int num = 0;
		for (int i = 0; i < log_superobj_num; i++)
		{
			if (pow(log_superobj_x[i] - log_x, 2) + pow(log_superobj_y[i] - log_y, 2) < min)
			{
				min = static_cast<int>(pow(log_superobj_x[i] - log_x, 2) + pow(log_superobj_y[i] - log_y, 2));
				num = i;
			}
		}

		if (min < 1200)
		{
			//I delete log_superobj_x[2], I have to move the data of log_superobj_x[3] to [2].
			for (int i = num + 1; i < log_superobj_num; i++)
			{
				log_superobj_x[i] = log_superobj_x[i - 1];
				log_superobj_y[i] = log_superobj_y[i - 1];
			}
			loaded_objects[SUPER_LOADED_ID]++;
			log_superobj_num--;
		}
		else
		{
			// Take it as a precaution, but do not increase loaded objects
			setAction(MAY_SUPER_FIND);
			SuperDuration += 6;
		}
	}
	else if (Duration > 0)
	{
		Duration--;
	}
	else if (IsOnYellowLine() && LoadedObjects > 0) 
	{
		if (IsOnSwampland())
		{
			if (IsOnYellowLine() == LEFT)
			{
				motor(-20, -100);
			}
			else if (IsOnYellowLine() == RIGHT)
			{
				motor(-100, -20);
			}
			else
			{
				motor(-100, -20);
			}
			Duration = 15;
		}
		else
		{
			if (IsOnYellowLine() == LEFT)
			{
				motor(-60, -1000);
			}
			else if (IsOnYellowLine() == RIGHT)
			{
				motor(-100, -60);
			}
			else
			{
				motor(-100, -100);
			}
			Duration = 3;
		}
	}
	else if (IsOnDepositArea() && (LoadedObjects >= 6 || (LoadedObjects > 0 && Time > 310)))
	{
		process = 0;
		large_process = -1;
		process_times = 0;
		if (IsOnDepositArea() == 3)
		{
			setAction(DEPOSIT_OBJ);
			Duration = kFindObjDuration + 10;
		}
		else if (IsOnDepositArea() == LEFT)
		{
			motor(0, 60);
		}
		else
		{
			motor(60, 0);
		}
	}
	else if (LoadedObjects >= 6 || (Time > 330 && log_superobj_num == 0 && (LoadedObjects > 2 || loaded_objects[SUPER_LOADED_ID] > 0)))
	{
		searching_object = -1;
		GoInDots(180, 135, 180, 135, POINT_DEPOSIT);
	}
	else if (log_superobj_num > 0)
	{
		searching_object = SUPER_LOADED_ID;
		if (GoToDots(log_superobj_x[0], log_superobj_y[0], 0, 0))
		{
			if (prev_repeated_num + 40 < getRepeatedNum())
			{
				same_time = 0;
			}
			if (same_time > 7)
			{
				log_superobj_num = 0;
				//LOG_MESSAGE("There is no superobj", MODE_NORMAL);
			}
			GoToPosition(log_superobj_x[0] - 5 + rand() % 10, log_superobj_y[0] - 5 + rand() % 10, 1, 1, 1);
			same_time++;
		}
	}
	// Movement in zone
	else //TODO: сделать езду по зонам в отдельной функции (Done, возможны ошибки)
	{
		if (loaded_objects[RED_LOADED_ID] < kBorderSameObjNum)
		{
			GoToArea(RED_LOADED_ID);
		}
		else if (loaded_objects[CYAN_LOADED_ID] < kBorderSameObjNum)
		{
			GoToArea(CYAN_LOADED_ID);
		}
		else
		{
			GoToArea(BLACK_LOADED_ID);
		}
	}

	switch (static_cast<int>(getAction()))
	{
	case DEFINED:
		//defined motor power by motor(int left, int right)
		break;
	case FIND_OBJ:
		if (Duration == kFindObjDuration || SuperDuration == kFindObjDuration)
		{
			LoadedObjects++;
		}
		LED_1 = 1;
		MyState = 0;
		WheelLeft = 0;
		WheelRight = 0;
		if (Duration == 0 && SuperDuration == 0)
		{
			LED_1 = 0;
		}
		break;
	case MAY_SUPER_FIND:
		LED_1 = 1;
		MyState = 0;
		WheelLeft = 0;
		WheelRight = 0;
		if (Duration == 0 && SuperDuration == 0)
		{
			LED_1 = 0;
		}
		if (Duration < 4 && SuperDuration < 4) {
			motor(60, 60);
		}
		break;
	case DEPOSIT_OBJ:
		WheelLeft = 0;
		WheelRight = 0;
		LED_1 = 2;
		MyState = 0;
		LoadedObjects = 0;
		//initialize all value of loaded_objects
		for (int i = 0; i < 4; i++)
		{
			loaded_objects[i] = 0;
		}
		if (Duration <= 6 && SuperDuration <= 6)
		{
			LED_1 = 0;
			motor(-100, -100);
		}
		else
		{
			if (IsOnDepositArea() != 3)
			{
				LoadedObjects = 6;
				Duration = 0;
				SuperDuration = 0;
				for (int i = 1; i < 4; i++)
				{
					loaded_objects[i] = 2;
				}
				setAction(DEFINED);
			}
		}
		break;
	default:
		break;
	}

	super_sameoperate++;
	if (log_superobj_num == 0)
	{
		super_sameoperate = 0;
	}
	if (getAction() == FIND_OBJ || getAction() == DEPOSIT_OBJ) {
		super_sameoperate -= 3;
	}
	if (super_sameoperate > 800)
	{
		log_superobj_num = 0;
		super_sameoperate = 0;
	}

	//double seconds = pt.end();
	//LOG_MESSAGE("loop time :" + to_string(seconds) + " ms", MODE_NORMAL);
}

long Game1_Hikaru::WhereIsColorSensor(void)
{
	long x, y;
	if (log_x < 0 || log_x >= kCospaceWidth || log_y < 0 || log_y >= kCospaceHeight)
	{
		log_x = kCospaceWidth / 2;
		log_y = kCospaceHeight / 2;
	}
	x = log_x + static_cast<long>(cos((Compass + 90) * 3.14 / 180) * 5);
	y = log_y + static_cast<long>(sin((Compass + 90) * 3.14 / 180) * 5);
	if (x < 0)
	{
		x = 0;
	}
	if (y < 0)
	{
		y = 0;
	}
	if (x >= kCospaceWidth)
	{
		x = kCospaceWidth - 1;
	}
	if (y >= kCospaceHeight)
	{
		y = kCospaceHeight - 1;
	}
	return y * 1000 + x;
}

void Game1_Hikaru::CheckNowDot(void)
{
	// If the left color sensor, right color sensor, and coordinate center point are A, B, C respectively,
	// Triangle ABC is a regular triangle with sides 5-6
	// x and y are the x and y coordinates of the left color sensor, coordinate sensor location, and right color sensor location, respectively
	int side = 6;
	int x[3] = {
		static_cast<int>(log_x + cos((Compass + 90 + 30) * M_PI / 180) * side),
		log_x,
		static_cast<int>(log_x + cos((Compass + 90 - 30) * M_PI / 180) * side) };
	int y[3] = {
		static_cast<int>(log_y + sin((Compass + 90 + 30) * M_PI / 180) * side),
		log_y,
		static_cast<int>(log_y + sin((Compass + 90 - 30) * M_PI / 180) * side) };

	rep(i, 3)
	{
		// out of range
		if (x[i] < 0)
			x[i] = 0;
		if (x[i] >= kCospaceWidth)
			x[i] = kCospaceWidth - 1;
		if (y[i] < 0)
			y[i] = 0;
		if (y[i] >= kCospaceHeight)
			y[i] = kCospaceHeight - 1;

		x[i] /= kSize;
		y[i] /= kSize;
		if (x[i] < 0)
		{
			x[i] = 0;
		}
		if (x[i] >= kDotWidthNum)
		{
			x[i] = kDotWidthNum - 1;
		}
		if (y[i] < 0)
		{
			y[i] = 0;
		}
		if (y[i] >= kDotHeightNum)
		{
			y[i] = kDotHeightNum - 1;
		}
		// If the dot is a wall
		if (dot[y[i] * kDotWidthNum + x[i]].point == POINT_WALL)
		{
			// Make a nearby dot that is not in the wall
			int range = (10 + kSize - 1) / kSize;
			int min_position[2] = { -1, -1 };
			int min_value = INT_MAX;
			for (int hi = -range; hi <= range; hi++)
			{
				for (int wj = -range; wj <= range; wj++)
				{
					int temp_x = x[i] + wj, temp_y = y[i] + hi;
					if (temp_y < 0 || temp_y >= kCospaceHeight || temp_x < 0 || temp_x >= kCospaceWidth)
					{
						continue;
					}
					if (dot[temp_y * kDotWidthNum + temp_x].point != POINT_WALL)
					{
						if (min_value > ABS(hi) + ABS(wj))
						{
							min_value = ABS(hi) + ABS(wj);
							min_position[0] = temp_x;
							min_position[1] = temp_y;
						}
					}
				}
			}
			if (min_position[0] < 0)
			{
				// y * kDotWidth + x -> -1
				ERROR_MESSAGE(FUNCNAME + "(): i = " + to_string(i) + " there is no not wall dot near " + to_string(x[i]) + " " + to_string(y[i]), MODE_NORMAL);

				y[i] = static_cast<int>(emergency_now_dot_id / kDotWidthNum);
				x[i] = emergency_now_dot_id - y[i] * kDotWidthNum;
			}
		}
		dot_x[i] = x[i];
		dot_y[i] = y[i];
	}
	now_dot_id = dot_y[1] * kDotWidthNum + dot_x[1];
}

int Game1_Hikaru::IsNearYellow(int num, int x, int y)
{
	num = num * 2 + 1;
	if (x < 0)
	{
		x = dot_x[1];
		y = dot_y[1];
	}

	for (int i = 0; i < num * num; i++)
	{
		if (i == num * num / 2)
		{
			continue;
		}

		int temp_x, temp_y;
		temp_y = i / num;
		temp_x = i % num;

		temp_x -= num / 2;
		temp_x += x;
		temp_y -= num / 2;
		temp_y += y;

		if (temp_x < 0 || temp_x >= kDotWidthNum || temp_y < 0 || temp_y >= kDotHeightNum)
		{
			continue;
		}

		int id = temp_y * kDotWidthNum + temp_x;
		if (dot[id].point == POINT_YELLOW)
		{
			return 1;
		}
	}
	if (map_secure[SECURE_YELLOW][y * kDotWidthNum + x] == 1) {
		return 1;
	}
	return 0;
}

int Game1_Hikaru::GoToPosition(int x, int y, int wide_decide_x, int wide_decide_y, int wide_judge_arrived)
{
	//LOG_MESSAGE(FUNCNAME + "(" + to_string(x) + ", " + to_string(y) + ", " + to_string(wide_decide_x) + ", " + to_string(wide_decide_y) + ", " + to_string(wide_judge_arrived) + "): start", MODE_DEBUG);
	static int absolute_x = -1;
	static int absolute_y = -1;
	static int absolute_distance = -1;
	static int same_operate = -1;

	static int repeated_num_log = -1;
	static int objects_num_log = -1;
	if (repeated_num_log == -1)
	{
		repeated_num_log = getRepeatedNum() - 1;
		objects_num_log = LoadedObjects;
	}

	// If the argument value is strange
	if (x < 0 || y < 0 || x > kCospaceWidth || y > kCospaceHeight || wide_decide_x < 0 || wide_decide_y < 0 || wide_judge_arrived < 0)
	{
		//printf("GoToPosition(): Argument is (%d, %d, %d, %d, %d)\n", x, y, wide_decide_x, wide_decide_y, wide_judge_arrived);
		return 0;
	}

	// If the absolute_x, y value is deviated from the x, y value
	if (absolute_x == -1 || !(PLUSMINUS(absolute_x, x, wide_decide_x) && PLUSMINUS(absolute_y, y, wide_decide_y)))
	{
		int i = 0;
		do
		{
			if (i > 20)
			{
				absolute_x = x;
				absolute_y = y;
				ERROR_MESSAGE("warming GoToPosition(): absolute_x, absolute_yが決まりません; (" + to_string(x) + ", " + to_string(y) + ", " + to_string(wide_decide_x) + ", " + to_string(wide_decide_y) + ", " + to_string(wide_judge_arrived) + ")", MODE_NORMAL);
				break;
			}
			absolute_x = x - wide_decide_x + (rnd() + 1) % (wide_decide_x * 2 + 1);
			absolute_y = y - wide_decide_y + (rnd() + 1) % (wide_decide_y * 2 + 1);
			i++;
		} while (absolute_x < 10 || absolute_x > kCospaceWidth - 10 || absolute_y < 10 || absolute_y > kCospaceHeight - 10);
		//same_operate = 0;
	}

	if (absolute_distance <= -1)
	{
		absolute_distance = static_cast<int>(sqrt(pow(absolute_x - log_x, 2) + pow(absolute_y - log_y, 2))) + 40;
	}

	if (absolute_distance < same_operate)
	{
		//printf("(%d, %d) arrive because too many same_operate\n", x, y);
		absolute_x = -1;
		absolute_y = -1;
		same_operate = -1;
		absolute_distance = -1;
		if (PositionX == -1)
		{
			log_x = x;
			log_y = y;
		}
		return 1;
	}

	int temp_x = WhereIsColorSensor();
	int temp_y = temp_x / 1000;
	temp_x -= temp_y * 1000;
	if (PLUSMINUS(absolute_x, temp_x, wide_judge_arrived) && PLUSMINUS(absolute_y, temp_y, wide_judge_arrived))
	{
		//printf("(%d, %d)Arrived at\n", absolute_x, absolute_y);
		//LOG_MESSAGE("(" + to_string(absolute_x) + "," + to_string(absolute_y) + ")Arrived at", MODE_NORMAL);
		absolute_x = -1;
		absolute_y = -1;
		same_operate = -1;
		absolute_distance = -1;
		return 1;
	}

	//LOG_MESSAGE("ab(" + to_string(absolute_x) + "," + to_string(absolute_y) + ")", MODE_NORMAL);
	x = absolute_x;
	y = absolute_y;
	x = x - temp_x;
	y = y - temp_y;
	//LOG_MESSAGE("x, y = " + to_string(x) + ", " + to_string(y), MODE_NORMAL);
	double angle = atan2(y, x);
	angle = angle * 180 / 3.14;
	int angle_int = static_cast<int>(angle);
	angle_int -= 90;
	if (angle_int < 0)
	{
		angle_int += 360;
	}
	//LOG_MESSAGE("angle " + to_string(angle_int), MODE_NORMAL);
	GoToAngle(angle_int, static_cast<int>(sqrt(x * x + y * y)));

	if (repeated_num_log + 1 == getRepeatedNum() && objects_num_log != LoadedObjects)
	{
		same_operate++;
	}
	else
	{
		same_operate = 0;
	}
	repeated_num_log = getRepeatedNum();

	return 0;
}

void Game1_Hikaru::InputDotInformation(void)
{
	for (int i = 0; i < kDotWidthNum; i++)
	{
		for (int j = 0; j < kDotHeightNum; j++)
		{
			switch (map_output_data[kDotHeightNum - j - 1][i])
			{
			case 0: //white or unlown
				map_position_color_data[i][j] = POINT_UNKNOWN;
				//map_position_color_data[i][j] = POINT_WHITE;
				break;
			case 1: //yellow
				map_position_color_data[i][j] = POINT_YELLOW;
				break;
			case 2: //wall
				map_position_color_data[i][j] = POINT_WALL;
				break;
			case 3: //swampland
				map_position_color_data[i][j] = POINT_SWAMPLAND;
				break;
			case 4: //deposit
				map_position_color_data[i][j] = POINT_DEPOSIT;
				break;
			case 5: // super area
				map_position_color_data[i][j] = POINT_SUPERAREA;
				break;
			default:
				map_position_color_data[i][j] = POINT_WHITE;
				break;
			}
		}
	}

	for (long i = 0; i < kMaxDotNum; i++)
	{
		//I use id of dot
		//id = y * 36(= 360 / kSize) + x;
		//x and y are 360 / kSize and 270 / kSize
		//kSize may be 10

		int x, y;
		//kDotWidthNum = 360 / kSize
		y = i / kDotWidthNum;
		x = i - y * kDotWidthNum;

		dot[i].id = i;
		//x position. if x = 0, center of dot is 0 * 10(=kSize) + 5(=kSize / 2)
		dot[i].x = x * kSize + kSize / 2;
		dot[i].y = y * kSize + kSize / 2;
		//the wide of dot
		dot[i].wide = kSize;
		//point means what's this dot belongs?
		//For example, this dot belongs yellow.
		//this map_position_color_data is defined at 60 lines upper(may be)
		dot[i].point = map_position_color_data[x][y];
		// dot[i].point = 1;
		dot[i].color = map_position_color_data[x][y];
		dot[i].red = red_data[kDotHeightNum - y - 1][x];
		dot[i].cyan = cyan_data[kDotHeightNum - y - 1][x];
		dot[i].black = black_data[kDotHeightNum - y - 1][x];
	}

	//set values of cost
	for (long i = 0; i < kMaxDotNum; i++)
	{
		int y = i / kDotWidthNum;
		int x = i - y * kDotWidthNum;
		dot[i].edge_num = 0;
		for (int j = 0; j < 9; j++)
		{
			if (j == 4)
			{
				continue;
			}
			int temp_x, temp_y;
			temp_y = j / 3;
			temp_x = j - temp_y * 3;
			temp_x += x - 1;
			temp_y += y - 1;
			int target_id = temp_y * kDotWidthNum + temp_x;
			if (temp_x < 0 || temp_x >= kDotWidthNum || temp_y < 0 || temp_y >= kDotHeightNum)
			{
				continue;
			}
			dot[i].edge_to[dot[i].edge_num] = target_id;
			//yellow or wall
			if (dot[i].point <= POINT_WALL || dot[target_id].point <= POINT_WALL)
			{
				// dot[i].edge_cost[dot[i].edge_num] = -1;
				dot[i].edge_num--;
			}
			//swampland
			else if (dot[i].point == POINT_SWAMPLAND || dot[target_id].point == POINT_SWAMPLAND || dot[i].point == POINT_MAY_SWAMPLAND || dot[target_id].point == POINT_MAY_SWAMPLAND)
			{
				dot[i].edge_cost[dot[i].edge_num] = (dot[i].wide + dot[target_id].wide) * 1000 * 1000;
			}
			//Map edge
			else if (x == 0 || y == 0 || x == kDotWidthNum - 1 || y == kDotHeightNum - 1 || temp_x == 0 || temp_y == 0 || temp_x == kDotWidthNum - 1 || temp_y == kDotHeightNum - 1)
			{
				dot[i].edge_cost[dot[i].edge_num] = (dot[i].wide + dot[target_id].wide) * 1000 * 1000;
			}
			else
			{
				//others
				dot[i].edge_cost[dot[i].edge_num] = (dot[i].wide + dot[target_id].wide) / 2;
			}
			if (j % 2 == 0 && dot[i].edge_num >= 0 && dot[i].point != POINT_SWAMPLAND && dot[i].point != POINT_MAY_SWAMPLAND && dot[target_id].point != POINT_SWAMPLAND && dot[target_id].point != POINT_MAY_SWAMPLAND)
			{
				dot[i].edge_cost[dot[i].edge_num] = static_cast<int>(dot[i].edge_cost[dot[i].edge_num] * 1.4);
			}
			dot[i].edge_num++;
		}
	}
}

void Game1_Hikaru::Dijkstra(int option)
{
	//fprintfLOL(logfile, " %d Start Dijkstra()\n", getRepeatedNum());
	for (int i = 0; i < kMaxDotNum; i++)
	{
		dot[i].id = i;
		dot[i].cost = -1;
		// dot[i].distance_from_start = -1;
		dot[i].done = -1;
		dot[i].from = -1;
	}

	int now_node_id = now_dot_id;

	if (dot[now_dot_id].point == POINT_SWAMPLAND || dot[now_dot_id].point == POINT_MAY_SWAMPLAND) {

		option = 1;
	}
	if (dot[now_dot_id].point == POINT_YELLOW || dot[now_dot_id].point == POINT_WALL) {
		option = 2;
	}

	if (now_node_id < 0 || now_node_id >= kMaxDotNum)
	{
		ERROR_MESSAGE(FUNC_NAME + "(); now dot id value is " + to_string(now_dot_id), MODE_NORMAL);
		return;
	}
	dot[now_node_id].cost = 0;
	dot[now_node_id].from = now_node_id;

	struct Dot investigating_node;

	// int number = 0;
	while (true)
	{
		// number++;
		//investigating_nodeを初期化
		investigating_node.done = 0;
		for (int i = 0; i < kMaxDotNum; i++)
		{
			//if done is 0, it means already
			if (dot[i].done == 0 || dot[i].cost < 0)
			{
				continue;
			}

			// if (dot[i].point < -1 || (dot[i].point < 0 && option == 0)) {
			// 	continue;
			// }

			//If the dot is yellow or wall
			if (option != 2 && (dot[i].point == POINT_WALL || dot[i].point == POINT_YELLOW))
			{
				continue;
			}

			if (option == 0 && (dot[i].point == POINT_SWAMPLAND 
								|| dot[i].point == POINT_MAY_SWAMPLAND)) 
			{
				continue;
			}
			//If not assigned
			if (investigating_node.done == 0)
			{
				investigating_node = dot[i];
				continue;
			}
			//If the cost of the new dot is small
			if (dot[i].cost < investigating_node.cost)
			{
				investigating_node = dot[i];
			}
		}
		//For a new node
		if (investigating_node.done == 0)
		{
			break;
		}

		dot[investigating_node.id].done = 0;
		for (int i = 0; i < investigating_node.edge_num; i++)
		{
			int target_id = investigating_node.edge_to[i];
			if (target_id < 0 || target_id >= kMaxDotNum)
			{
				continue;
			}

			if (dot[target_id].done == 0)
			{
				continue;
			}

			//If target_dot is yellow or wall
			if (option != 2 && (dot[i].point == POINT_WALL || dot[i].point == POINT_YELLOW))
			{
				continue;
			}

			if (option == 0 && (dot[i].point == POINT_SWAMPLAND || dot[i].point == POINT_MAY_SWAMPLAND)) {
				continue;
			}

			int remember_from = dot[target_id].from;
			dot[target_id].from = investigating_node.id;
			int target_curved_times = HowManyCurved(target_id);
			int target_cost = investigating_node.cost + investigating_node.edge_cost[i];

			target_cost += target_curved_times * 10;

			if (dot[i].point == POINT_SWAMPLAND || dot[i].point == POINT_MAY_SWAMPLAND)
			{
				//cout << "s" << endl;
				target_cost *= 1000;
			}

			if (dot[i].point == POINT_WALL || dot[i].point == POINT_YELLOW)
			{
				target_cost *= 10000;
			}

			double k = 0.8;
			if (Time < 330) {
				if (dot[investigating_node.id].black == 1 && loaded_objects[BLACK_LOADED_ID] < kBorderSameObjNum)
				{
					if (searching_object == BLACK_LOADED_ID)
					{
						target_cost = static_cast<int>((k - 0.3) * target_cost);
					}
					target_cost = static_cast<int>(k * target_cost);
				}
				if (dot[investigating_node.id].cyan == 1 && loaded_objects[CYAN_LOADED_ID] < kBorderSameObjNum)
				{
					if (searching_object == CYAN_LOADED_ID)
					{
						target_cost = static_cast<int>((k - 0.3) * target_cost);
					}
					target_cost = static_cast<int>(k * target_cost);
				}
				if (dot[investigating_node.id].red == 1 && loaded_objects[RED_LOADED_ID] < kBorderSameObjNum)
				{
					if (searching_object == RED_LOADED_ID)
					{
						target_cost = static_cast<int>((k - 0.3) * target_cost);
					}
					target_cost = static_cast<int>(k * target_cost);
				}
				if (LoadedObjects < 6)
				{
					target_cost += static_cast<int>(dot[target_id].arrived_times * 10);
				}
			}

			if (target_cost <= 0)
			{
				target_cost = 1;
			}

			if (dot[target_id].cost < 0 || target_cost < dot[target_id].cost)
			{
				dot[target_id].cost = target_cost;
				// dot[target_id].distance_from_start = investigating_node.distance_from_start + investigating_node.edge_cost[i];
				dot[target_id].from = investigating_node.id;
			}
			else
			{
				dot[target_id].from = remember_from;
			}
		}
	}
}

int Game1_Hikaru::GoToDot(int x, int y)
{
	//printf("%d %d\n", x * kSize, y * kSize);
	static int prev_x = -1, prev_y = -1, prev_now_dot_id = -1;

	//fprintfLOL(logfile, " %d Start GoToDot(%d, %d)\n", getRepeatedNum(), x, y);
	if (PositionX == -1 && (PLUSMINUS(log_x, x * kSize, kSize) && PLUSMINUS(log_y, y * kSize, kSize)))
	{
		//fprintfLOL(logfile, " %d End GoToDot() with returning 1 because I am in PLA and it's near target(%d, %d)\n", getRepeatedNum(), x, y);
		//LOG_MESSAGE(FUNC_NAME + "() end returning 1 because I am in PLA and it's near target(" + to_string(x) + ", " + to_string(y) + ")", MODE_NORMAL);
		GoToPosition(x, y, 10, 10, 5);
		return 1;
	}
	char map_data_to_show[kMaxDotNum]; 
	for (int i = 0; i < kMaxDotNum; i++)
	{
		if (dot[i].point == POINT_YELLOW)
		{
			map_data_to_show[i] = 'Y';
		}
		else if (dot[i].point == POINT_WALL)
		{
			map_data_to_show[i] = '#';
		}
		else if (dot[i].point == POINT_DEPOSIT)
		{
			map_data_to_show[i] = 'D';
		}
		else if (dot[i].point == POINT_SWAMPLAND || dot[i].point == POINT_MAY_SWAMPLAND)
		{
			map_data_to_show[i] = '$';
		}
		else if (dot[i].point == POINT_SUPERAREA)
		{
			map_data_to_show[i] = 'S';
		}
		else if (dot[i].point == POINT_UNKNOWN)
		{
			map_data_to_show[i] = '\'';
		}
		else
		{
			map_data_to_show[i] = ' ';
		}
	}
	//If the node I want to go will be go out
	if (x < 1 || x >= kDotWidthNum - 1 || y < 1 || y >= kDotHeightNum - 1)
	{
		//LOG_MESSAGE(FUNC_NAME + "(): (x, y) is (" + to_string(x) + ", " + to_string(y) + "and strange", MODE_NORMAL);
	}

	if (prev_now_dot_id != now_dot_id || prev_x != x || prev_y != y)
	{
		if (dot[y * kDotWidthNum + x].point == POINT_SWAMPLAND || dot[y * kDotWidthNum + x].point == POINT_MAY_SWAMPLAND)
		{
			Dijkstra(1);
		}
		else 
		{
			Dijkstra(0);
		}
	}
	prev_now_dot_id = now_dot_id;
	prev_x = x;
	prev_y = y;

	// //printf("from %d %d to %d %d\n", now_dot_id - (int)(now_dot_id / kDotWidthNum) * kDotWidthNum, now_dot_id / kDotWidthNum, x, y);

	int goal_dot = y * kDotWidthNum + x;

	if (goal_dot < 0 || goal_dot >= kMaxDotNum)
	{
		//LOG_MESSAGE("strainge(x, y)", MODE_NORMAL);
		return 0;
	}

	int temp = goal_dot;
	map_data_to_show[goal_dot] = 'T';
	int i = 0;

	while (dot[temp].from != now_dot_id && i < 200)
	{
		temp = dot[temp].from;
		map_data_to_show[temp] = '+';
		//printf("%d\n", dot[temp].point);
		i++;
		if (temp < 0 || temp >= kMaxDotNum)
		{
			//LOG_MESSAGE("temp = " + to_string(temp) + "is strange. I will continue", MODE_NORMAL);
			GoToPosition(x * kSize, y * kSize, 5, 5, 5);
			return 0;
		}
	}
	if (i == 200)
	{
		//printf("\n\n\niの値が200ですByGoToNode()\n\n\n\n");
		//LOG_MESSAGE(FUNC_NAME + "(): iの値が200です", MODE_NORMAL);
	}

	map_data_to_show[now_dot_id] = '@';

	int next_x, next_y;
	next_y = temp / kDotWidthNum;
	next_x = temp - next_y * kDotWidthNum;

	int now_y = now_dot_id / kDotWidthNum;
	int now_x = now_dot_id - now_y * kDotWidthNum;
	// Output map in file
	if (getRepeatedNum() % 3 == 0)
	{
		//cout << "out map" << endl;
		ProcessingTime pt2;
		pt2.start();
		FILE* fp = fopen("map_out.txt", "w");
		if (fp == NULL)
		{
			ERROR_MESSAGE(FUNCNAME + "(): failed to make map_out.txt", MODE_NORMAL);
		}
		else
		{
			//cout << "out map start" << endl;
			rep(xj, kDotWidthNum + 2)
			{
				fprintf(fp, "#");
				//printfLOL("#");
			}
			fprintf(fp, "\n");
			//printf("\n");
			rep(yi, kDotHeightNum)
			{
				fprintf(fp, "#");
				//printfLOL("#");

				rep(xj, kDotWidthNum)
				{
					fprintf(fp, "%c", map_data_to_show[(kDotHeightNum - 1 - yi) * kDotWidthNum + xj]);
					//printfLOL("%c", map_data_to_show[(kDotHeightNum - 1 - yi) * kDotWidthNum + xj]);
				}
				fprintf(fp, "#");
				//printfLOL("#");

				fprintf(fp, "\n");
				//printfLOL("\n");
			}
			rep(xj, kDotWidthNum + 2)
			{
				fprintf(fp, "#");
				//printfLOL("\n");
			}
			fprintf(fp, "\n");
			//printfLOL("\n");
			fclose(fp);
			//cout << "out map end " << pt2.end() << endl;
		}
	}

	int distance = 20;
	if (next_x < now_x)
	{
		if (next_y < now_y)
		{
			GoToAngle(135, distance);
		}
		else if (next_y == now_y)
		{
			GoToAngle(90, distance);
		}
		else
		{
			GoToAngle(45, distance);
		}
	}
	else if (next_x == now_x)
	{
		if (next_y < now_y)
		{
			GoToAngle(180, distance);
		}
		else if (next_y == now_y)
		{
			GoToPosition(log_x - 3 + rand() % 6, log_y - 3 + rand() % 6, 6, 6, 3);
			return 1;
		}
		else
		{
			GoToAngle(0, distance);
		}
	}
	else
	{
		if (next_y < now_y)
		{
			GoToAngle(225, distance);
		}
		else if (next_y == now_y)
		{
			GoToAngle(270, distance);
		}
		else
		{
			GoToAngle(315, distance);
		}
	}
	return 0;
}

int Game1_Hikaru::GoToDots(int x, int y, int wide_decide_x, int wide_decide_y)
{
	
	static int prev_x = -1;
	static int prev_y = -1;
	static int target_x = -1;
	static int target_y = -1;
	static int local_same_target = 0;
	static int same_target_border = 0;
	static int prev_loaded_num = -1;
	if (x != prev_x || y != prev_y)
	{
		//LOG_MESSAGE("changed dots", MODE_NORMAL);
		local_same_target = 0;
		prev_x = x;
		prev_y = y;
		//0:left bottom corner 1:right bottom corner 2:right bottom corner
		int corner_x[2], corner_y[2];
		corner_x[0] = (x - wide_decide_x) / kSize;
		corner_y[0] = (y - wide_decide_y) / kSize;
		corner_x[1] = (x + wide_decide_x) / kSize;
		corner_y[1] = (y + wide_decide_y) / kSize;

		for (int i = 0; i < 2; i++)
		{
			if (corner_x[i] < 0)
			{
				//fprintfLOL(logfile, " %d GoToDots() corner_x[%d] is %d < 0\n", getRepeatedNum(), i, corner_x[i]);
				corner_x[i] = 0;
			}
			if (corner_x[i] >= kDotWidthNum)
			{
				//fprintfLOL(logfile, " %d GoToDots() corner_x[%d] is %d >= %d\n", getRepeatedNum(), i, corner_x[i], kDotWidthNum);
				corner_x[i] = kDotWidthNum - 1;
			}
			if (corner_y[i] < 0)
			{
				//fprintfLOL(logfile, " %d GoToDots() corner_y[%d] is %d < 0\n", getRepeatedNum(), i, corner_y[i]);
				corner_y[i] = 0;
			}
			if (corner_y[i] >= kDotHeightNum)
			{
				//fprintfLOL(logfile, " %d GoToDots() corner_y[%d] is %d >= %d\n", getRepeatedNum(), i, corner_y[i], kDotHeightNum);
				corner_y[i] = kDotHeightNum - 1;
			}
		}

		int min = 100000, id = -1;
		const int N = 5;
		// Move once every N times
		int option = rnd() % N;
		for (int i = corner_x[0]; i <= corner_x[1]; i++)
		{
			for (int j = corner_y[0]; j <= corner_y[1]; j++)
			{
				int investigating_dot_id = j * kDotWidthNum + i;
				if (i <= 0 || i >= kDotWidthNum - 1 || j <= 0 || j >= kDotHeightNum - 1)
				{
					continue;
				}
				//yellow or wall or deposit
				if (dot[investigating_dot_id].point < POINT_DEPOSIT)
				{
					continue;
				}

				int costs = static_cast<int>(dot[investigating_dot_id].arrived_times * 100 + rnd() % 10);
				if (option)
				{
					// When not moving
					int k = 30;
					costs += static_cast<int>(pow(abs(i * kSize - log_x) - k, 2) * 100 - pow(abs(j * kSize - log_y) - k, 2) * 100);
				}
				else
				{
					// When to moving
					costs -= static_cast<int>(pow(i * kSize - log_x, 2) / 100 - pow(j * kSize - log_y, 2) / 100);
				}

				if (costs < min)
				{
					min = costs;
					id = investigating_dot_id;
				}
			}
		}
		if (id == -1)
		{
			//fprintfLOL(logfile, " %d GoToDots(): There is no dot that can go log(%d %d) eme %d\n", getRepeatedNum());
			target_x = x / kSize;
			target_y = y / kSize;
		}
		else
		{
			target_y = id / kDotWidthNum;
			target_x = id - target_y * kDotWidthNum;
		}

		same_target_border = static_cast<int>(sqrt(pow(log_x - target_x * kSize, 2) + pow(log_y - target_y * kSize, 2)));
		same_target_border *= 3;
		same_target_border += 40;
	}
	local_same_target++;
	if (prev_loaded_num != LoadedObjects) {
		prev_loaded_num = LoadedObjects;
		local_same_target -= 100;
	}

	if (GoToDot(target_x, target_y) || local_same_target > same_target_border)
	{
		prev_x = -1;
		local_same_target = 0;
		//fprintfLOL(logfile, " %d End GoToDots() with returning 1\n", getRepeatedNum());
		return 1;
	}
	else
	{
		//fprintfLOL(logfile, " %d End GoToDots() with returning 0\n", getRepeatedNum());
		return 0;
	}
}

int Game1_Hikaru::GoInDots(int x, int y, int wide_decide_x, int wide_decide_y, int color)
{
	//LOG_MESSAGE(FUNCNAME + "(): start", MODE_DEBUG);
	//fprintfLOL(logfile, " %d Start GoToDots(%d, %d, %d, %d)\n", getRepeatedNum(), x, y, wide_decide_x, wide_decide_y);
	//printf("GoToDots(): %d %d %d %d\n", x, y, wide_decide_x, wide_decide_y);
	static int prev_x = -1;
	static int prev_y = -1;
	static int prev_color = -1000;
	static int target_x = -1;
	static int target_y = -1;
	static int local_same_target = 0;
	static int same_target_border = 0;
	if (x != prev_x || y != prev_y || color != prev_color)
	{
		//LOG_MESSAGE("changed dots", MODE_NORMAL);
		local_same_target = 0;
		prev_x = x;
		prev_y = y;
		//0:left bottom corner 1:right bottom corner 2:right bottom corner
		int corner_x[2], corner_y[2];
		corner_x[0] = (x - wide_decide_x) / kSize;
		corner_y[0] = (y - wide_decide_y) / kSize;
		corner_x[1] = (x + wide_decide_x) / kSize;
		corner_y[1] = (y + wide_decide_y) / kSize;

		for (int i = 0; i < 2; i++)
		{
			if (corner_x[i] < 0)
			{
				corner_x[i] = 0;
			}
			if (corner_x[i] >= kDotWidthNum)
			{
				corner_x[i] = kDotWidthNum - 1;
			}
			if (corner_y[i] < 0)
			{
				corner_y[i] = 0;
			}
			if (corner_y[i] >= kDotHeightNum)
			{
				corner_y[i] = kDotHeightNum - 1;
			}
		}

		int min = 100000, id = -1;
		const int N = 3;
		// Move once every N times
		int option = rnd() % N;
		if (color == POINT_DEPOSIT) 
		{
			Dijkstra(0);
		}
		for (int i = corner_x[0]; i <= corner_x[1]; i++)
		{
			for (int j = corner_y[0]; j <= corner_y[1]; j++)
			{
				int investigating_dot_id = j * kDotWidthNum + i;
				if (i <= 0 || i >= kDotWidthNum - 1 || j <= 0 || j >= kDotHeightNum - 1)
				{
					continue;
				}
				//yellow or wall or deposit
				if (dot[investigating_dot_id].point == POINT_YELLOW 
					|| dot[investigating_dot_id].point == POINT_MAY_SWAMPLAND 
					|| dot[investigating_dot_id].point == POINT_SWAMPLAND 
					|| dot[investigating_dot_id].point == POINT_WALL)
				{
					continue;
				}
				if (color == POINT_RED)
				{
					if (dot[investigating_dot_id].red != 1)
					{
						continue;
					}
				}
				else if (color == POINT_CYAN)
				{
					if (dot[investigating_dot_id].cyan != 1)
					{
						continue;
					}
				}
				else if (color == POINT_BLACK)
				{
					if (dot[investigating_dot_id].black != 1)
					{
						continue;
					}
				}
				else if (color == POINT_DEPOSIT)
				{
					if (dot[investigating_dot_id].point != POINT_DEPOSIT)
					{
						continue;
					}
				}

				int costs = static_cast<int>(dot[investigating_dot_id].arrived_times * 100 + rnd() % 10);

				if (option)
				{
					// When not moving
					int k = 30;
					costs += static_cast<int>(sqrt(pow(abs(i * kSize - log_x) - k, 2) 
												+ pow(abs(j * kSize - log_y) - k, 2)));
				}
				else
				{
					// When to move
					costs -= static_cast<int>(pow(i * kSize - log_x, 2) / 100 - pow(j * kSize - log_y, 2) / 100);
				}
				if (color == POINT_DEPOSIT)
				{
					costs = dot[j * kDotWidthNum + i].cost;
				}
				if (costs < min)
				{
					min = costs;
					id = investigating_dot_id;
				}
			}
		}
		if (id == -1)
		{
			target_x = x / kSize;
			target_y = y / kSize;
		}
		else
		{
			target_y = id / kDotWidthNum;
			target_x = id - target_y * kDotWidthNum;
			//LOG_MESSAGE("target(" + to_string(target_x) + ", " + to_string(target_y), MODE_NORMAL);
		}

		same_target_border = static_cast<int>(sqrt(pow(log_x - target_x * kSize, 2) + pow(log_y - target_y * kSize, 2)));
		same_target_border *= 2;
		same_target_border += 30;
	}

	prev_x = x;
	prev_y = y;
	prev_color = color;

	local_same_target++;
	//LOG_MESSAGE("target_x, y " + to_string(target_x * kSize) + " " + to_string(target_y * kSize), MODE_NORMAL);
	if (GoToDot(target_x, target_y) || local_same_target > same_target_border)
	{
		prev_x = -1;
		local_same_target = 0;
		return 1;
	}
	else
	{
		return 0;
	}
}

int Game1_Hikaru::HowManyCurved(int id)
{
	/*
		Path length * 10 + number of turns * 20 + (when Object <6) Estimated calculation of Object
	*/
	int route[kMaxDotNum];
	//Number of revolutions
	int curved_times = 0;
	//Path length
	int distance_way = -1;
	route[0] = id;
	for (int i = 1; i < kMaxDotNum; i++)
	{
		if (route[i - 1] < 0 || route[i - 1] > kMaxDotNum)
		{
			distance_way = i + 1;
			break;
		}
		route[i] = dot[route[i - 1]].from;
		//We decide to put -1 in the last two of the route <-lie
		if (route[i] == now_dot_id)
		{
			distance_way = i + 1;
			break;
		}

		//When the number of dots is exceeded
		if (route[i] >= kMaxDotNum || route[i] < 0)
		{
			distance_way = i;
			break;
		}
	}
	int x[kMaxDotNum], y[kMaxDotNum], direction[kMaxDotNum];
	//direction Is Top left = 0, +1 on the right, +3 on the bottom
	if (distance_way >= kMaxDotNum)
	{
		distance_way = kMaxDotNum - 2;
	}
	y[0] = route[0] / kDotWidthNum;
	x[0] = route[0] - y[0] * kDotWidthNum;
	direction[0] = -1;
	for (int i = 1; i < distance_way; i++)
	{
		y[i] = route[i] / kDotWidthNum;
		x[i] = route[i] - y[i] * kDotWidthNum;
		switch (x[i] - x[i - 1])
		{
		case -1:
			direction[i] = 0;
			break;
		case 0:
			direction[i] = 1;
			break;
		case 1:
			direction[i] = 2;
			break;
		default:
			direction[i] = 5;
			break;
		}
		if (y[i] - y[i - 1] == 0)
		{
			direction[i] += 3;
		}
		else if (y[i] - y[i - 1] <= 0)
		{
			direction[i] += 6;
		}
		if (direction[i] != direction[i - 1])
		{
			curved_times++;
		}
	}
	return curved_times;
}

void Game1_Hikaru::GoToAngle(int angle, int distance)
{
	//LOG_MESSAGE(FUNCNAME + "(" + to_string(angle) + "," + to_string(distance) + "): start", MODE_VERBOSE);
	angle = angle - Compass;

	angle %= 360;

	if (angle > 180)
	{
		angle -= 360;
	}
	if (angle < -180)
	{
		angle += 360;
	}

	int classification = obstacle(8, 10, 8);
	if (abs(WheelLeft) + abs(WheelRight) < 6) 
	{
		classification = obstacle(8, 10, 8);
	}
	if (log_superobj_num > 0)
	{
		classification = obstacle(5, 7, 5);
	}
	if (LoadedObjects >= 6 || (Time > 340 && LoadedObjects > 1)) 
	{
		classification = obstacle(5, 7, 4);
	}

	int big_motor = 100;
	int short_motor = 40;
	if (IsNearYellow(1, -1, -1))
	{
		big_motor = 60;
		short_motor = 20;
	}
	if (IsOnSwampland())
	{
		big_motor = 100;
		if (!IsOnRedObj() && !IsOnCyanObj() && !IsOnBlackObj() && !IsOnSuperObj() && log_superobj_num == 0)
		{
			Duration += 20;
		}
	}
	switch (classification)
	{
	case 0:
		classification = obstacle(35, 35, 35);
		if (log_superobj_num > 0 && abs(log_superobj_x[0] - log_x) + abs(log_superobj_y[0] - log_y) < 400)
		{
			classification = obstacle(15, 20, 15);
		}
		if (classification == 1 && angle > 0 && angle < 90)
		{ //left
			motor(big_motor, short_motor);
		}
		else if (classification == 2 && abs(angle) < 30)
		{ //front
			if (angle < 0)
			{
				motor(big_motor, short_motor);
			}
			else
			{
				motor(short_motor, big_motor);
			}
		}
		else if (classification == 3 && angle > -30 && angle < 90)
		{ //left & front
			motor(big_motor, short_motor);
		}
		else if (classification == 4 && angle < 0 && angle > -90)
		{ //right
			motor(short_motor, big_motor);
		}
		else if (classification == 5 && abs(angle) > 30)
		{ //left & right
			if (abs(angle) < 150)
			{
				motor(big_motor, big_motor);
			}
			else
			{
				if (angle < 0)
				{
					motor(big_motor, -big_motor);
				}
				else
				{
					motor(-big_motor, big_motor);
				}
			}
		}
		else if (classification == 6 && angle < 30 && angle > -90)
		{ //front & right
			motor(short_motor, big_motor);
		}
		else if (classification == 7)
		{ //all
			if (angle < 0)
			{
				motor(big_motor, short_motor);
			}
			else
			{
				motor(short_motor, big_motor);
			}
		}
		else
		{
			if (log_superobj_num > 0 && pow(log_y - log_superobj_y[0], 2) + pow(log_x - log_superobj_x[0], 2) < 400)
			{
				if (abs(angle) < 30)
				{
					if (distance < 3 + static_cast<int>(rnd() % 5))
					{
						motor(-big_motor, -big_motor);
					}
					else
					{
						motor(big_motor, big_motor);
					}
					if (IsOnSwampland()) {
						Duration = 10;
					}
				}
				else if (abs(angle) < 100)
				{
					if (angle < 0)
					{
						motor(big_motor, -short_motor);
					}
					else
					{
						motor(-short_motor, big_motor);
					}
				}
				else if (abs(angle) < 120)
				{
					if (angle < 0)
					{
						motor(-short_motor, -big_motor);
					}
					else
					{
						motor(-big_motor, -short_motor);
					}
				}
				else
				{
					if (angle < 0)
					{
						motor(short_motor, -big_motor);
					}
					else
					{
						motor(-big_motor, short_motor);
					}
				}
			}
			else if (IsOnSwampland())
			{
				if (IsOnSwampland() == 1 && angle > 0 && angle < 90)
				{
					// left
					motor(100, 20);
				}
				else if (IsOnSwampland() == 2 && angle < 0 && angle > -90)
				{
					motor(20, 100);
				}
				else
				{

					if (abs(angle) < 30)
					{
						motor(100, 100);
					}
					else if (abs(angle) < 90)
					{
						if (angle < 0)
						{
							motor(100, 0);
						}
						else
						{
							motor(0, 100);
						}
					}
					else
					{
						if (angle < 0)
						{
							motor(100, -100);
						}
						else
						{
							motor(-100, 100);
						}
					}
				}

				Duration = 5;
			}
			else if (IsNearYellow(2, -1, -1) && LoadedObjects != 0)
			{
				if (abs(angle) < 10)
				{
					motor(60, 60);
				}
				else if (abs(angle) < 30)
				{
					if (angle < 0)
					{
						motor(80, 60);
					}
					else
					{
						motor(60, 80);
					}
				}
				else if (abs(angle) < 80 && LoadedObjects != 6)
				{
					if (angle < 0)
					{
						motor(80, 40);
					}
					else
					{
						motor(40, 80);
					}
				}
				else if (abs(angle) < 120)
				{
					if (angle < 0)
					{
						motor(80, 20);
					}
					else
					{
						motor(20, 80);
					}
				}
				else
				{
					if (angle < 0)
					{
						motor(60, -60);
					}
					else
					{
						motor(-60, 60);
					}
				}
			}
			else if ((loaded_objects[BLACK_LOADED_ID] < kBorderSameObjNum && dot[now_dot_id].black == 1)
				|| (loaded_objects[CYAN_LOADED_ID] < kBorderSameObjNum && dot[now_dot_id].cyan == 1))
			{
				if (abs(angle) < 10)
				{
					motor(80, 80);
				}
				else if (abs(angle) < 80)
				{
					if (angle < 0)
					{
						motor(80, 40);
					}
					else
					{
						motor(40, 80);
					}
				}
				else if (abs(angle) < 120)
				{
					if (angle < 0)
					{
						motor(80, 0);
					}
					else
					{
						motor(0, 80);
					}
				}
				else
				{
					if (angle < 0)
					{
						motor(60, -60);
					}
					else
					{
						motor(-60, 60);
					}
				}
			}
			else
			{
				if (abs(angle) < 30)
				{
					if (distance < 20)
					{
						if (angle < 0)
						{
							motor(100, 60);
						}
						else
						{
							motor(60, 100);
						}
					}
					else
					{
						if (dot[now_dot_id].point == POINT_WHITE && IsOnSwampland())
						{
							if (IsOnSwampland() == 1)
							{
								motor(100, 20);
							}
							else if (IsOnSwampland() == 2)
							{
								motor(20, 100);
							}
						}
						else
						{
							if (abs(angle) < 10) {
								motor(100, 100);
							}
							else if (angle < 0) {
								motor(100, 80);
							}
							else {
								motor(80, 100);
							}
						}
					}
				}
				else if (abs(angle) < 80)
				{
					if (angle < 0)
					{
						motor(100, 60);
					}
					else
					{
						motor(60, 100);
					}
				}
				else if (abs(angle) < 120)
				{
					if (angle < 0)
					{
						motor(100, 20);
					}
					else
					{
						motor(20, 100);
					}
				}
				else if (abs(angle) < 180) {
					if (angle < 0)
					{
						motor(40, -40);
					}
					else
					{
						motor(-40, 40);
					}
				}
				else
				{
					if (angle < 0)
					{
						motor(100, -100);
					}
					else
					{
						motor(-100, 100);
					}
				}
			}
		}
		break;
	case 1: //left
		if (IsOnSwampland())
		{
			motor(-20, -100);
		}
		else
		{

			motor(-40, -80);
		}
		break;
	case 2: //front
		if (IsOnSwampland())
		{
			if (US_Left < US_Right)
			{
				motor(-20, -100);
			}
			else
			{
				motor(-100, -20);
			}
		}
		else
		{

			if (US_Left < US_Right)
			{
				motor(-20, -60);
			}
			else
			{
				motor(-60, -20);
			}
		}
		break;
	case 3: //left front
		if (IsOnSwampland())
		{
			motor(-20, -100);
		}
		else
		{
			motor(-40, -80);
		}
		break;
	case 4: //right
		if (IsOnSwampland())
		{
			motor(-100, -20);
		}
		else
		{
			motor(-80, -40);
		}
		break;
	case 5: //left right
		if (IsOnSwampland())
		{
			motor(-100, -100);
		}
		else
		{
			motor(-60, -60);
		}
		break;
	case 6: //front right
		if (CurGame == 0)
		{
			motor(60, 60);
		}
		else
		{
			if (IsOnSwampland())
			{
				motor(-20, -100);
			}
			else
			{
				motor(-40, -60);
			}
		}
		break;
	case 7: //left front right
		if (IsOnSwampland())
		{
			motor(-100, -100);
		}
		else
		{
			motor(-60, -60);
		}
		break;
	default:
		break;
	}
}

void Game1_Hikaru::saveColorInfo(void)
{
	/*
	Top
	POINT_DEPOSIT
	POINT_YELLOW
	POINT_SWAMPLAND

	Medium
	POINT_SUPERAREA
	POINT_WHITE

	Lower rank
	POINT_MAY_SWAMPLAND
	POINT_UNKNWON

	Other
	POINT_WALL
	 */
	if (ColorJudgeLeft(object_box2))
	{
		dot[dot_y[0] * kDotWidthNum + dot_x[0]].point = POINT_DEPOSIT;
		dot[dot_y[0] * kDotWidthNum + dot_x[0]].color = POINT_DEPOSIT;
	}
	else if (ColorJudgeLeft(trap_line))
	{
		int range = 1;
		if (PositionX == -1)
		{
			range = 2;
		}
		int left = 0, right = 0, up = 0, down = 0;
		if (Compass < 180)
		{
			left += range;
		}
		else
		{
			right += range;
		}
		if (90 <= Compass && Compass < 270)
		{
			down += range;
		}
		else
		{
			up += range;
		}
		for (int yi = dot_y[0] - down; yi <= dot_y[0] + up; ++yi)
		{
			if (yi < 0 || yi >= kDotHeightNum)
			{
				continue;
			}
			for (int xj = dot_x[0] - left; xj <= dot_x[0] + right; ++xj)
			{
				if (xj < 0 || xj >= kDotWidthNum)
				{
					continue;
				}

				if (PositionX == -1)
				{
					dot[yi * kDotWidthNum + xj].point = POINT_MAY_SWAMPLAND;
					dot[yi * kDotWidthNum + xj].color = POINT_MAY_SWAMPLAND;
				}
				else
				{
					if (yi != dot_y[0] && xj != dot_x[0])
					{
						continue;
					}					
					dot[yi * kDotWidthNum + xj].point = POINT_YELLOW;
					dot[yi * kDotWidthNum + xj].color = POINT_YELLOW;
				}
			}
		}
		dot[dot_y[0] * kDotWidthNum + dot_x[0]].point = POINT_YELLOW;
		dot[dot_y[0] * kDotWidthNum + dot_x[0]].color = POINT_YELLOW;
	}
	else if (ColorJudgeLeft(gray_zone))
	{
		for (int yi = dot_y[0] - 1; yi <= dot_y[0] + 1; ++yi)
		{
			if (yi < 0 || yi >= kDotHeightNum)
			{
				continue;
			}
			for (int xj = dot_x[0] - 1; xj <= dot_x[0] + 1; ++xj)
			{
				if (xj < 0 || xj >= kDotWidthNum)
				{
					continue;
				}
				if (dot[yi * kDotWidthNum + xj].point == POINT_UNKNOWN)
				{
					dot[yi * kDotWidthNum + xj].point = POINT_MAY_SWAMPLAND;
					dot[yi * kDotWidthNum + xj].color = POINT_MAY_SWAMPLAND;
				}
			}
		}
			dot[dot_y[0] * kDotWidthNum + dot_x[0]].point = POINT_SWAMPLAND;
			dot[dot_y[0] * kDotWidthNum + dot_x[0]].color = POINT_SWAMPLAND;
	}
	else if (ColorJudgeLeft(blue_zone))
	{
		if (dot[dot_y[0] * kDotWidthNum + dot_x[0]].point != POINT_DEPOSIT && dot[dot_y[0] * kDotWidthNum + dot_x[0]].point != POINT_YELLOW && dot[dot_y[0] * kDotWidthNum + dot_x[0]].point != POINT_SWAMPLAND)
		{
			dot[dot_y[0] * kDotWidthNum + dot_x[0]].point = POINT_SUPERAREA;
			dot[dot_y[0] * kDotWidthNum + dot_x[0]].color = POINT_SUPERAREA;
		}
	}
	else
	{
		if (PositionX >= 180 || (dot[dot_y[0] * kDotWidthNum + dot_x[0]].point != POINT_DEPOSIT && map_position_color_data[dot_x[0]][dot_y[0]] != POINT_SWAMPLAND))
		{
			dot[dot_y[0] * kDotWidthNum + dot_x[0]].point = POINT_WHITE;
			dot[dot_y[0] * kDotWidthNum + dot_x[0]].color = POINT_WHITE;
		}
	}

	if (ColorJudgeRight(object_box2))
	{
		dot[dot_y[2] * kDotWidthNum + dot_x[2]].point = POINT_DEPOSIT;
		dot[dot_y[2] * kDotWidthNum + dot_x[2]].color = POINT_DEPOSIT;
	}
	else if (ColorJudgeRight(trap_line))
	{
		int range = 1;
		int left = 0, right = 0, up = 0, down = 0;
		if (PositionX == -1)
		{
			range = 2;
		}
		if (Compass < 180)
		{
			left += range;
		}
		else
		{
			right += range;
		}
		if (90 <= Compass && Compass < 270)
		{
			down += range;
		}
		else
		{
			up += range;
		}
		for (int yi = dot_y[0] - down; yi <= dot_y[0] + up; ++yi)
		{
			if (yi < 0 || yi >= kDotHeightNum)
			{
				continue;
			}
			for (int xj = dot_x[0] - left; xj <= dot_x[0] + right; ++xj)
			{
				if (xj < 0 || xj >= kDotWidthNum)
				{
					continue;
				}

				if (PositionX == -1)
				{
					dot[yi * kDotWidthNum + xj].point = POINT_MAY_SWAMPLAND;
					dot[yi * kDotWidthNum + xj].color = POINT_MAY_SWAMPLAND;
				}
				else
				{
					if (yi != dot_y[2] && xj != dot_x[2])
					{
						continue;
					}
					dot[yi * kDotWidthNum + xj].point = POINT_YELLOW;
					dot[yi * kDotWidthNum + xj].color = POINT_YELLOW;
				}
			}
		}
		dot[dot_y[2] * kDotWidthNum + dot_x[2]].point = POINT_YELLOW;
		dot[dot_y[2] * kDotWidthNum + dot_x[2]].color = POINT_YELLOW;
	}
	else if (ColorJudgeRight(gray_zone))
	{
		for (int yi = dot_y[2] - 1; yi <= dot_y[2] + 1; ++yi)
		{
			if (yi < 0 || yi >= kDotHeightNum)
			{
				continue;
			}
			for (int xj = dot_x[2] - 1; xj <= dot_x[2] + 1; ++xj)
			{
				if (xj < 0 || xj >= kDotWidthNum)
				{
					continue;
				}
				if (dot[yi * kDotWidthNum + xj].point == POINT_UNKNOWN)
				{
					dot[yi * kDotWidthNum + xj].point = POINT_MAY_SWAMPLAND;
					dot[yi * kDotWidthNum + xj].color = POINT_MAY_SWAMPLAND;
				}
			}
		}
		dot[dot_y[2] * kDotWidthNum + dot_x[2]].point = POINT_SWAMPLAND;
		dot[dot_y[2] * kDotWidthNum + dot_x[2]].color = POINT_SWAMPLAND;
	}
	else if (ColorJudgeRight(blue_zone))
	{
		if (dot[dot_y[2] * kDotWidthNum + dot_x[2]].point != POINT_DEPOSIT && dot[dot_y[2] * kDotWidthNum + dot_x[2]].point != POINT_YELLOW && dot[dot_y[2] * kDotWidthNum + dot_x[2]].point != POINT_SWAMPLAND)
		{
			dot[dot_y[2] * kDotWidthNum + dot_x[2]].point = POINT_SUPERAREA;
			dot[dot_y[2] * kDotWidthNum + dot_x[2]].color = POINT_SUPERAREA;
		}
	}
	else
	{
		if (PositionX >= 180 || (dot[dot_y[2] * kDotWidthNum + dot_x[2]].point != POINT_DEPOSIT && map_position_color_data[dot_x[2]][dot_y[2]] != POINT_SWAMPLAND))
		{
			dot[dot_y[2] * kDotWidthNum + dot_x[2]].point = POINT_WHITE;
			dot[dot_y[2] * kDotWidthNum + dot_x[2]].color = POINT_WHITE;
		}
	}
}

void Game1_Hikaru::calculateWallPosition(void)
{
	if (PositionX != -1)
	{
		// 0: left & right 1: front
		int difference_us_position[2] = { 9, 9 };
		int us_sensors[3] = { US_Left, US_Front, US_Right };
		int angles[3] = { 40, 0, -40 };
		int calculated_relative_coordinate[3][2];
		int calculated_absolute_dot_position[3][2];
		for (int i = 0; i < 3; ++i)
		{

			angles[i] += Compass + 90;
			angles[i] %= 360;
			if (us_sensors[i] > kUSLimit - 1)
			{
				us_sensors[i] = kUSLimit;
			}
			us_sensors[i] += difference_us_position[i % 2];
			//Wall position and robot relative coordinates
			calculated_relative_coordinate[i][0] = static_cast<int>(cos(angles[i] * M_PI / 180) * us_sensors[i]);
			calculated_relative_coordinate[i][1] = static_cast<int>(sin(angles[i] * M_PI / 180) * us_sensors[i]);

			// Absolute coordinates of the wall on the dot
			calculated_absolute_dot_position[i][0] = static_cast<int>((log_x + calculated_relative_coordinate[i][0] + kSize / 2) / kSize);
			calculated_absolute_dot_position[i][1] = static_cast<int>((log_y + calculated_relative_coordinate[i][1] + kSize / 2) / kSize);
			if (0 <= calculated_absolute_dot_position[i][0] && calculated_absolute_dot_position[i][0] < kDotWidthNum && 0 <= calculated_absolute_dot_position[i][1] && calculated_absolute_dot_position[i][1] < kDotHeightNum)
			{
				// When there is no wall, there is no need to register MAP_WALL
				if (us_sensors[i] < kUSLimit + difference_us_position[i % 2])
				{
					if(dot[calculated_absolute_dot_position[i][1] * kDotWidthNum + calculated_absolute_dot_position[i][0]].point == POINT_UNKNOWN)
					{
						dot[calculated_absolute_dot_position[i][1] * kDotWidthNum + calculated_absolute_dot_position[i][0]].point = POINT_WALL;
					}
				}
				if(dot[calculated_absolute_dot_position[i][1] * kDotWidthNum + calculated_absolute_dot_position[i][0]].point == POINT_WALL)
				{
					dot[calculated_absolute_dot_position[i][1] * kDotWidthNum 
					+ calculated_absolute_dot_position[i][0]].timeWalls += wall_is_there;
				}
			}
		}
		
		for(int check_line = us_sensors[1]; check_line > difference_us_position[1]; check_line -= kSize)
		{
			ERROR_MESSAGE("Start", MODE_NORMAL);
			//Wall position and robot relative coordinates
			int natured_x = static_cast<int>
			(cos(angles[1] * M_PI / 180) * check_line);

			int natured_y = static_cast<int>
			(sin(angles[1] * M_PI / 180) * check_line);

			// Absolute coordinates of the wall on the dot
			int absolute_x = static_cast<int>((log_x + natured_x + kSize / 2) / kSize);
			if (absolute_x > kDotWidthNum)
			{
				absolute_x = kDotWidthNum - 1;
			}
			if(absolute_x < 0)
			{
				absolute_x = 0;
			}

			int absolute_y = static_cast<int>((log_y + natured_y + kSize / 2) / kSize);
			if (absolute_y > kDotHeightNum)
			{
				absolute_y = kDotHeightNum - 1;
			}
			if(absolute_y < 0)
			{
				absolute_y = 0;
			}

			if(dot[absolute_y * kDotWidthNum + absolute_x].point == POINT_WALL)
			{
				dot[absolute_y * kDotWidthNum + absolute_x].timeWalls -= wall_is_not_there;

				if(dot[absolute_y * kDotWidthNum + absolute_x].timeWalls < 0)
				{
					dot[absolute_y * kDotWidthNum + absolute_x].timeWalls = 0;
				}
			}
			
			if(dot[absolute_y * kDotWidthNum + absolute_x].timeWalls == 0 
			&& dot[absolute_y * kDotWidthNum + absolute_x].point == POINT_WALL)
			{
				dot[absolute_y * kDotWidthNum + absolute_x].point = POINT_UNKNOWN; 
			}
			ERROR_MESSAGE("End", MODE_NORMAL);
		}
	}
}

int Game1_Hikaru::goInArea(int x, int y, int wide_decide_x, int wide_decide_y, int times) {
	static int prev_x = -1, prev_y = -1, prev_wide_x = -1, prev_wide_y = -1;
	static int arrived_times = 0;
	static int log_compass = 0;
	static int is_turning = 0;
	if (x < 0 || x >= kCospaceWidth || y < 0 || y >= kCospaceHeight) {
		ERROR_MESSAGE(FUNCNAME + "(): x or y range is invalid", MODE_NORMAL);
	}
	if (prev_x != x || prev_y != y || prev_wide_x != wide_decide_x || prev_wide_y != wide_decide_y) {
		GoToDots(x, y, wide_decide_x, wide_decide_y);
		prev_x = x;
		prev_y = y;
		prev_wide_x = wide_decide_x;
		prev_wide_y = wide_decide_y;
		arrived_times = 0;
		is_turning = 0;
	}

	int range = 5;
	if (wide_decide_x <= range) {
		ERROR_MESSAGE(FUNCNAME + "rase wide_x", MODE_NORMAL);
		wide_decide_x = range + 1;
	}
	if (wide_decide_y <= range) {
		ERROR_MESSAGE(FUNCNAME + "rase wide_y", MODE_NORMAL);
		wide_decide_y = range + 1;

	}


	if (PLUSMINUS(log_x, x, wide_decide_x) && PLUSMINUS(log_y, y, wide_decide_y)) {
		if (PLUSMINUS(log_x, x, wide_decide_x - range) && PLUSMINUS(log_y, y, wide_decide_y - range)) {
			GoToAngle(log_compass - 3, 30);
			is_turning = 0;
		}
		else {
			motor(-40, 40);
			if (is_turning == 0) {

				arrived_times++;
				is_turning = 1;
			}
			Duration = 2;
		}
		if (PositionX == -1) {
			GoToAngle(180 + log_compass, 30);
		}
		else {
			log_compass = Compass;
		}
	}
	else {
		GoToDots(x, y, wide_decide_x, wide_decide_y);
	}

	if (arrived_times >= times) {
		return 1;
	}
	return 0;

}

void Game1_Hikaru::GoToArea(int color)
{
	const int X = 0;
	const int Y = 1; 
	int colorID;
	int firstArea[2];
	int secondArea[2];
	// TODO: Naming
	int supportCoordColor[2];
	if  (color  == RED_LOADED_ID)
	{
		colorID = RED_LOADED_ID;
		supportCoordColor[X] = toChooseRED[X];
		supportCoordColor[Y] = toChooseRED[Y];
		firstArea[X] = FirstCoordAriaRED[X];
		firstArea[Y] = FirstCoordAriaRED[Y];
		secondArea[X] = SecondcoordAriaRED[X];
		secondArea[Y] = SecondcoordAriaRED[Y];
	}
	else if(color  == BLACK_LOADED_ID)
	{
		colorID = BLACK_LOADED_ID;
		supportCoordColor[X] = toChooseBLACK[X];
		supportCoordColor[Y] = toChooseBLACK[Y];
		firstArea[X] = FirstCoordAriaBLACK[X];
		firstArea[Y] = FirstCoordAriaBLACK[Y];
		secondArea[X] = SecontCoordAriaBLACK[X];
		secondArea[Y] = SecontCoordAriaBLACK[Y];
	}
	else if(color  == CYAN_LOADED_ID)
	{
		colorID = CYAN_LOADED_ID;
		supportCoordColor[X] = toChooseCYACN[X];
		supportCoordColor[Y] = toChooseCYACN[Y];
		firstArea[X] = FirstCoordAriaCYAN[X];
		firstArea[Y] = FirstCoordAriaCYAN[Y];
		secondArea[X] = SecontCoordAriaCYAN[X];
		secondArea[Y] = SecontCoordAriaCYAN[Y];
	}
	if (loaded_objects[colorID] < kBorderSameObjNum)
	{
		if (large_process != 2 || next_allowed_go_time[colorID][process] > Time)
		{
			if (PositionX < supportCoordColor[X] && PositionY < supportCoordColor[Y] && next_allowed_go_time[colorID][0] <= Time) 
			{
				process = 1;
			}
			else if (!(PositionX < supportCoordColor[X] && PositionY < supportCoordColor[Y])) 
			{
				process = 0;
			}
			else 
			{
				if (next_allowed_go_time[colorID][1] < next_allowed_go_time[colorID][0]) {
					process = 1;
				}
				else {
					process = 0;
				}
			}
			process_times = 0;
		}

		if (process == 0)
		{
			if (GoInDots(firstArea[X], firstArea[Y], scatter[X], scatter[Y], color))
			{
				if (process_times >= 4)
				{
					next_allowed_go_time[colorID][process] = Time + skip_time;
					process++;
					process_times = 0;
				}
				process_times++;
			}
		}
		else  if(process == 1)
		{
			if (GoInDots(secondArea[X], secondArea[Y], scatter[X], scatter[Y], color))
			{
				if (process_times >= 3)
				{
					next_allowed_go_time[colorID][process] = Time + skip_time;
					process = 0;
					process_times = 0;
				}
				process_times++;
			}
		}
		else
		{
			process = 0;
			process_times = 0;
		}
		large_process = 2;
	}

}
