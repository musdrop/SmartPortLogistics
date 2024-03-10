#pragma once
#include <iostream>
#include <random>
#include <vector>
using namespace std;

#define N  200//地图大小
#define robot_num 10//机器人数量
#define berth_num 10//泊位数量
#define boat_num 5//船数量

//机器人
class Robot
{
private:
	int x, y;//当前x,y坐标
	int mbx, mby;//目标x,y坐标
	bool goods;//机器人是否携带货物
	bool status;//是否正常运行
public:
	Robot() {}
	void Set(int x, int y, int goods, int status);
};
//泊位
class Berth
{
private:
	int ltx, lty;//泊位左上角坐标
	int rbx, rby;//泊位右下角坐标
	int transport_time;//泊位到虚拟点的运输时间
	int loading_speed;//装货速度，每帧装货的货物数量
public:
	Berth() {}
	void Set(int x, int y, int transport_time, int loading_speed);
};
//船
class Boat
{
private:
	int status;//船状态,0表示移动(运输)中 1表示正常运行状态(即装货状态或运输完成状态) 2表示泊位外等待状态
	int pos;//船当前位置或目标位置，-1表示虚拟点，0-9表示泊位
public:
	Boat() {}
	void Set(int status, int pos);
};
//货物
class Goods
{
private:
	int id;//货物id
	int x, y;//货物坐标
	int val;//货物价值
	int status;//货物状态,0表示在地上 1表示被机器人搬运
	int surtime;//货物剩余存在帧数
public:
	Goods(int id = 0, int x = 0, int y = 0, int val = 0);
};
