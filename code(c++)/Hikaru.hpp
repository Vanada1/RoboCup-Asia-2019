#ifndef HIKARU
#define HIKARU

#include "CospaceSettings.hpp"
#include "CommonTools.hpp"
#include <random>
#include <ctime>

// World2
class Game1_Hikaru : public UserGame1
{
public:
	void setup(void) override;
	void loop(void) override;

private:
	/*
		Settings
	*/
// TODO: Naming
	int toChooseRED[2] = 		{240, 140};
	int toChooseBLACK[2] = 		{240, 140};
	int toChooseCYACN[2] = 		{240, 140};

	int FirstCoordAriaRED[2] = 		{240, 140};
	int SecondcoordAriaRED[2] = 	{240, 140};
	int FirstCoordAriaBLACK[2] = 	{240, 140};
	int SecontCoordAriaBLACK[2] = 	{240, 140};
	int FirstCoordAriaCYAN[2] = 	{240, 140};
	int SecontCoordAriaCYAN[2] = 	{240, 140};
	int scatter[2] = {60, 60};


	const static int kCospaceWidth = 360;
	const static int kCospaceHeight = 270;
	const static int kSize = 10;
	const static int kDotWidthNum = kCospaceWidth / kSize;
	const static int kDotHeightNum = kCospaceHeight / kSize;
	const static int kMaxDotNum = kCospaceWidth * kCospaceHeight / kSize / kSize;
	const static int kMaxEdgeNum = 25;
	const static int kBorderSameObjNum = 2;
	const static int kFindObjDuration = 48;
	int process_times = 0;
	int log_superobj_num, log_superobj_x[10], log_superobj_y[10];
	int now_dot_id;
	int emergency_now_dot_id = 292;
	int super_sameoperate = 0;
	int searching_object;
	int same_target_num = 5;
	int same_target = 0;
	const int kUSLimit = 185;
	int log_x = -1, log_y = -1;
	int map_position_color_data[36][27];
	int map_secure[7][kMaxDotNum];
	int next_allowed_go_time[4][5];
	int skip_time = 100;
	LogErrorMessage logErrorMessage;

	int dot_x[3], dot_y[3];

	int large_process = -1;
	int process = 0;

	std::random_device rnd;

	struct Dot
	{
		int x, y;  //x (0 <= x <36), y (0 <= y <27) coordinates of dot
		int wide;  //一辺の長さ
		int point; //Dot type(-3:yellow -2:wall etc.)
		int done;  //Dijkstra()
		long id;   //y * 36 + x
		int from;  //Dijkstra()
		int cost;  //Dijkstra()
		int is_opened;
		int score;
		int distance_from_start;              //Dijkstra()
		int curved_times;                     //Dijkstra()
		unsigned long long int arrived_times; //Number of times you have been there
		int edge_num;                         //The number of dots that can go to that dot
		int edge_to[kMaxEdgeNum];             //
		int edge_cost[kMaxEdgeNum];           //
		int red;                              //If you can get Red, 1
		int cyan;                             //0 if Cyan cannot be taken
		int black;                            //If Black ...
		int color;
		int near_swampland;
	};
	struct Dot dot[kMaxDotNum];

	char dot_status[kMaxDotNum];
	int dot_from[kMaxDotNum];
	int dot_cost[kMaxDotNum];
	int dot_estimated_cost[kMaxDotNum];
	int dot_edge_to[kMaxEdgeNum];
	int dot_edge_cost[kMaxEdgeNum];
	char dot_is_red[kMaxDotNum];
	char dot_is_cyan[kMaxDotNum];
	char dot_is_black[kMaxDotNum];

	void GoToAngle(int angle, int distance);
	int GoToPosition(int x, int y, int wide_decide_x, int wide_decide_y, int wide_judge_arrived);
	void Dijkstra(int option);
	void Astar(void);
	int GoToDot(int x, int y);
	void CheckNowDot(void);
	long WhereIsColorSensor(void);
	void InputDotInformation(void);
	int GoToDots(int x, int y, int wide_decide_x, int wide_decide_y);
	int GoInDots(int x, int y, int wide_decide_x, int wide_decide_y, int color);
	int HowManyCurved(int id);
	int IsNearYellow(int num, int x, int y);
	int goInArea(int x, int y, int wide_decide_x, int wide_decide_y, int times);
	void saveColorInfo(void);
	void calculateWallPosition(void);

	void CreateMap(char* map_data_to_show)
	void OutputMap(char* map_data_to_show);
	void GoToArea(int color);
};

#endif // !HIKARU
