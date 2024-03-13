#include "Statement.h"

int money;//当前金钱
int boat_capacity;//船的最大装货量,个数
int flushid;//当前帧id
char map[N][N];//港口地图
int total_goods = 0;//货物总数
vector<Goods> goods;//还未运到泊位的货物信息
unordered_map<int, bool> gdMap;//货物id与是否可被捡起的映射

Robot robot[robot_num];
Berth berth[real_berth_num];
Boat boat[boat_num];

int main()
{
	Logger::instance.Initialize(true, false, "LogMessage.txt");
	Manager manager;
	manager.Init();
	for (int zhen = 1; zhen <= 15000; zhen++)
	{
		//读取当前帧信息
		manager.Input();
		//每帧输出
		manager.React();
	}
	return 0;
}