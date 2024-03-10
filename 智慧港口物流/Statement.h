#pragma once
#include <iostream>
#include <random>
#include <vector>
#include <unordered_map>
using namespace std;

#define N 200//地图大小
#define robot_num 10//机器人数量
#define berth_num 10//泊位数量
#define boat_num 5//船数量

enum Towards//机器人移动方向
{
	Right,
	Left,
	Up,
	Down
};
class Robot;
class Goods;
class Berth;
//机器人
class Robot
{
private:
	int id;//机器人id
	int x, y;//当前x,y坐标
	bool status = true;//是否正常运行

	char preGround = '.';//上一帧机器人所在位置的地面类型
	vector<pair<int, int>> path;//机器人当前路径
	int curPathIndex = 0;//当前路径的下标
public:
	int tarGdId = -1;//目标货物id
	int isInPath = 0;//是否在路径中,-1去泊位，0是闲着，1去货物
private:
	bool Move(Towards tw);//基础移动
	Towards TwofNearPoint(int x, int y);//根据坐标移动到相邻点
	void PickUp();//捡货
	void PutDown();//放货
	int SelectGoods();//选择离自己最近的货物,返回id
	int SelectBerth();//选择离自己最近的泊位
	bool TargetGoods();//是否有目标货物
public:
	Robot();
	void ToGetGoods();//去拿货物
	void ToPutGoods();//去放货物
	void FlushPos();//每帧根据路径刷新位置
	void MoveTo(int x, int y);//设定到目标点的路径
	void Set(int id, int x, int y, int isCarrygoods, int status);
	void SetGoodsState(int id, bool state);//设置货物状态
};
//泊位
class Berth
{
private:
	int id;//泊位id
	int ltx, lty;//泊位左上角坐标
	int rbx, rby;//泊位右下角坐标
	int transport_time;//泊位到虚拟点的运输时间
	int loading_speed;//装货速度，每帧装货的货物数量
public:
	Berth();
	void Set(int id, int x, int y, int transport_time, int loading_speed);
	pair<int, int> GetAvailablePos();//获取泊位上的可用位置
};
//船
class Boat
{
private:
	int id;//船id
	int status;//船状态,0表示移动(运输)中 1表示正常运行状态(即装货状态或运输完成状态) 2表示泊位外等待状态
	int pos;//船当前位置或目标位置，-1表示虚拟点，0-9表示泊位
public:
	Boat();
	void Set(int id, int status, int pos);
};
//货物
class Goods
{
private:
	int val;//货物价值
	int status;//货物状态,0表示在地上 1表示被机器人搬运
	int surtime;//货物剩余存在帧数
public:
	int id;//货物id
	int x, y;//货物坐标
public:
	Goods(int id = 0, int x = 0, int y = 0, int val = 0);
	void LiveDown();//存活时间衰减
	bool IsAlive();//是否存活
};
//全局管理类
class Manager
{
private:
public:
	void Init();//初始化
	int Input();//每帧输入读取
	void ClearDeadGoods();//清除死亡货物
	void FlushOperation();//每帧固定操作
	void React();//每帧反应
};