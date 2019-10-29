#ifndef HIKARU
#define HIKARU

#include "CospaceSettings.hpp"
#include "LogErrorTools.hpp"
#include "CommonTools.hpp"
#include <random>
#include <ctime>

// World2
class Game1_Test2 : public UserGame1
{
public:
	void setup(void) override;
	void loop(void) override;
    
private:
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

	//LogErrorMessage //logErrorMessage;

class CospaceMap
    {
    public:
        // CospaceMap();
        enum MapInfo
        {
            MAP_FAILURE = -1000,
            MAP_SUCCESS = -1001,
            MAP_YELLOW = 0,    // Determined by the value of the color sensor
            MAP_SWAMPLAND = 1, // Determined by the value of the color sensor
            MAP_UNKNOWN = 2, // If unknown
            MAP_WALL = 3,
            MAP_WHITE = 4,      // Determined by the value of the color sensor
            MAP_DEPOSIT = 5,    // Determined by the value of the color sensor
            MAP_SUPER_AREA = 6, // Determined by the value of the color sensor
        };
        inline int setMapInfo(int x, int y, MapInfo info)
        {
            return setMapInfo(x, y, info, 1);
        }
        inline int setMapInfo(int x, int y, MapInfo info, int times)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            if (info == MAP_WALL)
            {
                map[map_wall_index][y][x] = times;
            }
            else if (info == MAP_WHITE && (map[0][y][x] != MAP_UNKNOWN && map[0][y][x] != MAP_SWAMPLAND))
            {
                return kFailure;
            }
            else if (info == MAP_SWAMPLAND && map[0][y][x] != MAP_UNKNOWN)
            {
                return kFailure;
            }
            else
            {
                // if(map[0][y][x] == MAP_UNKNOWN) {
                //     map[0][y][x] = info;
                // }
                // else {
                //     map[0][y][x] = info;
                // }
                map[0][y][x] = info;
            }
            return kSuccess;
        }
        inline int setMapInfoForce(int x, int y, MapInfo info)
        {
            return setMapInfoForce(x, y, info, 1);
        }
        inline int setMapInfoForce(int x, int y, MapInfo info, int times)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            if (info == MAP_WALL)
            {
                map[map_wall_index][y][x] = times;
            }
            else
            {
                map[0][y][x] = info;
            }
            return kSuccess;
        }
        inline int addMapInfo(int x, int y, MapInfo info)
        {
            return setMapInfo(x, y, info, 1);
        }
        inline int addMapInfo(int x, int y, MapInfo info, int times)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            if (info == MAP_WALL)
            {
                map[map_wall_index][y][x] += times;
            }
            else
            {
                // if(map[0][y][x] == MAP_UNKNOWN) {
                //     map[0][y][x] = info;
                // }
                // else {
                //     map[0][y][x] = info;
                // }
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + "(): warming; you should use setMapInfo() instead of " + FUNCNAME + "() if you don't want to change the value of MAP_WALL", MODE_NORMAL);
                }
                map[0][y][x] = info;
            }
            return kSuccess;
        }
        inline MapInfo getMapInfo(int x, int y)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return MAP_FAILURE;
            }
            if (map[map_wall_index][y][x] > 0)
            {
                return MAP_WALL;
            }
            else
            {
                return static_cast<MapInfo>(map[0][y][x]);
            }
            return MAP_SUCCESS;
        }
        inline int setMapObjInfo(int x, int y, int object_loaded_id)
        {
            return setMapObjInfo(x, y, object_loaded_id, 1);
        }
        inline int setMapObjInfo(int x, int y, int object_loaded_id, int value)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            if (object_loaded_id != RED_LOADED_ID && object_loaded_id != CYAN_LOADED_ID && object_loaded_id != BLACK_LOADED_ID)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; object_loaded_id = " + std::to_string(object_loaded_id), MODE_NORMAL);
                }
                return kFailure;
            }
            if (object_loaded_id <= 0 || object_loaded_id >= static_cast<int>((std::extent<decltype(map), 0>::value)))
            {
                if (MODE_VERBOSE <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; object_loaded_id = " + std::to_string(object_loaded_id), MODE_VERBOSE);
                }
                return kFailure;
            }
            map[object_loaded_id][y][x] = value;
            return kSuccess;
        }
        inline int getMapObjInfo(int x, int y, int object_loaded_id)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            if (object_loaded_id != RED_LOADED_ID && object_loaded_id != CYAN_LOADED_ID && object_loaded_id != BLACK_LOADED_ID)
            {
                if (MODE_VERBOSE <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; object_loaded_id = " + std::to_string(object_loaded_id), MODE_VERBOSE);
                }
                return kFailure;
            }
            if (object_loaded_id <= 0 || object_loaded_id >= static_cast<int>(std::extent<decltype(map), 0>::value))
            {
                if (MODE_VERBOSE <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; object_loaded_id = " + std::to_string(object_loaded_id), MODE_VERBOSE);
                }
                return kFailure;
            }
            return map[object_loaded_id][y][x];
        }
        inline int addMapArrivedTimes(int x, int y, int times)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            map_arrived_times[y][x] += times;
            return kSuccess;
        }
        inline int addMapArrivedTimes(int x, int y)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            return addMapArrivedTimes(x, y, 1);
        }
        inline int getMapArrivedTimes(int x, int y)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            return map_arrived_times[y][x];
        }
        inline int setMapArrivedTimes(int x, int y, int value)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            map_arrived_times[y][x] = value;
        }
        inline int setMapFrom(int x, int y, int from_x, int from_y)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            map_from[y][x][0] = from_x;
            map_from[y][x][1] = from_y;
            return kSuccess;
        }
        inline int getMapFrom(int x, int y, int *from_x, int *from_y)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            *from_x = map_from[y][x][0];
            *from_y = map_from[y][x][1];
            return kSuccess;
        }
        inline int setMapCost(int x, int y, int cost)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            map_cost[y][x] = cost;
            return kSuccess;
        }
        inline int getMapCost(int x, int y)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            return map_cost[y][x];
        }
        inline int setMapTotalCost(int x, int y, int cost)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            map_total_cost[y][x] = cost;
            return kSuccess;
        }
        inline int getMapTotalCost(int x, int y)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            return map_total_cost[y][x];
        }
        inline int setMapStatus(int x, int y, int status)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            map_status[y][x] = status;
            return kSuccess;
        }
        inline int getMapStatus(int x, int y)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            return map_status[y][x];
        }
        inline int setMapCurvedTimes(int x, int y, int value)
        {
            if (x < 0 || x >= kDotWidthNum || y < 0 || y >= kDotHeightNum)
            {
                if (MODE_NORMAL <= getRunMode())
                {
                    //logErrorMessage.errorMessage(FUNCNAME + " Failed; (x, y)=(" + std::to_string(x) + ", " + std::to_string(y) + ")", MODE_NORMAL);
                }
                return kFailure;
            }
            map_curved_times[y][x] = value;
            return kSuccess;
        }
        int getMapCurvedTimes(int from_x, int from_y, int target_x, int target_y);

        const static int kSuccess = -1;
        const static int kFailure = INT_MIN;
        const static int kGuessedMapSize = 10;

    private:
        const static int map_wall_index = 4;
        // 0:Floor information 1:red 2:cyan 3:black 4:Wall information
        int map[5][kDotHeightNum][kDotWidthNum];
        int map_arrived_times[kDotHeightNum][kDotWidthNum];
        int map_from[kDotHeightNum][kDotWidthNum][2];
        int map_cost[kDotHeightNum][kDotWidthNum];
        int map_total_cost[kDotHeightNum][kDotWidthNum];
        int map_status[kDotHeightNum][kDotWidthNum];
        int map_curved_times[kDotHeightNum][kDotWidthNum];
		//int map_secure[7][kSecureAreaHeight][kSecureAreaWidth];
    };
    CospaceMap cospaceMap;
    int dotsForInvestegation[5][2] = {{15, 15}, {345, 15}, {180, 135}, {60, 180}, {290, 190}};
    int processForInvestigation = 0;
    const static int kSuccess = -1;
    const static int kFailure = INT_MIN;
    const static int kGuessedMapSize = 10;

	int dot_x[3], dot_y[3];

	int large_process = -1;
	int process = 0;

	std::random_device rnd;

	struct Dot
	{
		int x, y;  //dotのx(0<=x<36), y(0<=y<27)座標
		int wide;  //Length of one side
		int point; //Dot type(-3:yellow -2:wall etc.)
		int done;  //Dijkstra()
		long id;   //y * 36 + x
		int from;  //Dijkstra()
		int cost;  //Dijkstra()
		int is_opened;
		int score;
		int distance_from_start;              //Dijkstra()
		int curved_times;                     //Dijkstra()
		unsigned long long int arrived_times; //The number of Ss I have been there
		int edge_num;                         //The number of dots that can go to that dot
		int edge_to[kMaxEdgeNum];             //
		int edge_cost[kMaxEdgeNum];           //
		int red;                              //If Red can be taken, 1
		int cyan;                             //0 if Cyan cannot be taken
		int black;                            //If Black...
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
};

#endif // !HIKARU
