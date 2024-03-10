#include "Statement.h"

int money;//当前金钱
int boat_capacity;//船的最大装货量,个数
int id;//当前帧id
char map[N][N];//港口地图
int total_goods = 0;//货物总数
vector<Goods> goods;//货物信息
unordered_map<int, bool> gdMap;//货物id与是否可被捡起的映射

Robot robot[robot_num];
Berth berth[berth_num];
Boat boat[boat_num];

int main()
{
	Manager manager;
	manager.Init();
	for (int zhen = 1; zhen <= 15000; zhen++)
	{
		//读取当前帧信息
		int id = manager.Input();
		//每帧输出
		manager.React();
	}
	return 0;
}




int a;