#include "Statement.h"
extern int money;//当前金钱
extern int boat_capacity;//船的最大装货量,个数
extern int id;//当前帧id
extern char map[N][N];//港口地图
extern int total_goods;//货物总数
extern vector<Goods> good;//货物信息

extern Robot robot[robot_num];
extern Berth berth[berth_num];
extern Boat boat[boat_num];

void Robot::Set(int x, int y, int goods, int status) 
{
	this->x = x;
	this->y = y;
	this->goods = goods;
	this->status = status;
}

void Berth::Set(int x, int y, int transport_time, int loading_speed)
{
	this->ltx = x;
	this->lty = y;
	this->transport_time = transport_time;
	this->loading_speed = loading_speed;
}


void Boat::Set(int status, int pos)
{
	this->pos = pos;
	this->status = status;
}

Goods::Goods(int id, int x, int y, int val) :status(0), surtime(1000)
{
	this->id = id;
	this->x = x;
	this->y = y;
	this->val = val;
}

