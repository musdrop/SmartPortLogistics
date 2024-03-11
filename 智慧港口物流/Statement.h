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
	Goods* tarGdPtr = NULL;//目标货物指针
	int tarBerthId = -1;//目标泊位id
	int isInPath = 0;//是否在路径中,-1去泊位，0是闲着，1去货物
private:
	bool Move(Towards tw);//基础移动
	Towards TwofNearPoint(int x, int y);//根据坐标移动到相邻点
	void PickUp();//捡货
	void PutDown();//放货
	Goods* SelectGoods();//选择离自己最近的货物,返回货物指针
	int SelectBerth();//选择离自己最近的泊位,返回下标
	void MoveTo(int x, int y);//设定到目标点的路径
	void ToGetGoods();//去拿货物
	void ToPutGoods();//去放货物
	void FlushPos();//每帧根据路径刷新位置
public:
	Robot();
	void FlushAction();//每帧行动决策
	void Set(int id, int x, int y, int isCarrygoods, int status);
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
	vector<Goods> berthGoods;//泊位上的货物
public:
	bool isBoatComing = false;//是否有船在来此的路上
	int boat_id;//当前泊位上的船id
public:
	Berth();
	void Set(int id, int x, int y, int transport_time, int loading_speed);
	pair<int, int> GetAvailablePos();//获取泊位上的可用位置
	void AddGoods(Goods* gdPtr);//泊位上增加货物
};
//船
class Boat
{
private:
	int id;//船id
	int status;//船状态,0表示移动(运输)中 1表示正常运行状态(即装货状态或运输完成状态) 2表示泊位外等待状态
	int pos;//船当前位置或目标位置，-1表示虚拟点，0-9表示泊位
private:
	int SelectBerth();//选择泊位,返回下标
	bool IsAvailable();//是否可用,状态为1且在虚拟点
	void ShipTo(int berthId);//命令船移动到目标泊位
	void GoToSell();//去虚拟点出售货物
	bool IsOkToSell();//判断是否应该去出售货物
	void ToLoadGoods();//去装货
public:
	Boat();
	void FlushAction();//每帧行动决策
	void Set(int id, int status, int pos);
};
//货物
class Goods
{
private:
	int status;//货物状态,0表示在地上 1表示被机器人搬运
	int birthflushid;//货物生成帧id
public:
	int id;//货物id
	int x, y;//货物坐标
	int val;//货物价值
public:
	Goods(int id = 0, int x = 0, int y = 0, int val = 0, int birthflushid = 0);
	bool IsAlive();//是否存活
};
//全局管理类
class Manager
{
private:
public:
	void Init();//初始化
	void Input();//每帧输入读取
	void ClearDeadGoods();//清除死亡货物
	void FlushOperation();//每帧固定操作
	void React();//每帧反应
};