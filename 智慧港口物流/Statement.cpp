#include "Statement.h"

extern int money;//当前金钱
extern int boat_capacity;//船的最大装货量,个数
extern int flushid;//当前帧id
extern char map[N][N];//港口地图
extern int total_goods;//货物总数
extern vector<Goods> goods;//货物信息
extern unordered_map<int, bool> gdMap;//货物id与是否可被捡起的映射

extern Robot robot[robot_num];
extern Berth berth[berth_num];
extern Boat boat[boat_num];

bool Robot::Move(Towards tw)
{
	int curx = x, cury = y;
	switch (tw)
	{
	case Right:
		cury++;
		break;
	case Left:
		cury--;
		break;
	case Up:
		curx--;
		break;
	case Down:
		curx++;
		break;
	}
	if (map[curx][cury] == 'B' || map[curx][cury] == '.')
	{
		map[x][y] = preGround;
		x = curx;
		y = cury;
		preGround = map[x][y];
		map[x][y] = 'A';
		cout << "move " << id << " " << tw << endl;
		return true;
	}
	else
	{
		return false;
	}
}

Towards Robot::TwofNearPoint(int x, int y)
{
	Towards tw;
	if (path[curPathIndex].first > x)
	{
		tw = Right;
	}
	else if (path[curPathIndex].first < x)
	{
		tw = Left;
	}
	else if (path[curPathIndex].second > y)
	{
		tw = Down;
	}
	else if (path[curPathIndex].second < y)
	{
		tw = Up;
	}
	return tw;
}

void Robot::PickUp()
{
	cout << "get " << id << endl;
	//将捡起的货物的可捡起状态置为false
	gdMap[tarGdPtr->id] = false;
}

void Robot::PutDown()
{
	cout << "pull " << id << endl;
	//将放入泊位的货物加到泊位的货物列表中
	berth[tarBerthId].AddGoods(tarGdPtr);
	//从待选货物列表中彻底移除放入泊位的货物
	goods.erase(goods.begin() + (tarGdPtr - &goods[0]));
	//将目标货物指针置空
	tarGdPtr = NULL;
	//将目标泊位id置空
	tarBerthId = -1;
}

Goods* Robot::SelectGoods()
{
	vector<pair<int, int>>nearGoods;//储存离机器人最近的三个货物的下标及距离
	for (int i = 0; i < goods.size(); i++)
	{
		if (gdMap[goods[i].id] == true)
		{
			int distance2;//机器人与货物的距离平方
			distance2 = (goods[i].x - x) * (goods[i].x - x) + (goods[i].y - y) * (goods[i].y - y);
			if (nearGoods.size() < 3)//当前可捡货物不足三件，直接添加至数组内
			{
				pair<int, int>a0;
				a0.first = i;
				a0.second = distance2;
				nearGoods.push_back(a0);
			}
			else
			{
				//寻找三个值中距离最长的与当前货物距离进行对比
				int max = 0;
				for (int j = 1; j < 3; i++)
				{
					if (nearGoods[j].second > nearGoods[max].second)
					{
						max = j;
					}
				}
				if (nearGoods[max].second > distance2)//若当前货物距离小于数组中最远的货物，则替换
				{
					nearGoods[max].first = i;
					nearGoods[max].second = distance2;
				}
			}
		}
	}
	if (nearGoods.size() == 0)//可选货物数量为0，返回空
	{
		return NULL;
	}
	int maxcostPerId = 0;//储存三个货物中性价比最高的货物下标
	int costPerformance0 = 0;//储存三个货物中最高的性价比（货物价值/距离的平方）
	for (int i = 0; i < nearGoods.size(); i++)
	{
		int costPerformance = goods[nearGoods[i].first].val / nearGoods[i].second;//储存当前的货物的性价比
		if (costPerformance > costPerformance0)//若当前货物性价比高于原定货物性价比，则替换
		{
			costPerformance0 = costPerformance;
			maxcostPerId = nearGoods[i].first;
		}
	}
	return &goods[maxcostPerId];//返回数组中性价比最高的货物的地址指针
}

int Robot::SelectBerth()
{
	int berthId = -1;
	int nearst = INT_MAX;
	for (int i = 0; i < berth_num; i++)
	{
		int temdis = berth[i].GetDistance(x, y);
		if (temdis < nearst)
		{
			nearst = temdis;
			berthId = i;
		}
	}
	return berthId;
}

Robot::Robot()
{
}

void Robot::ToGetGoods()
{
	tarGdPtr = SelectGoods();
	if (tarGdPtr == NULL)
	{
		isInPath = 0;
		return;
	}
	MoveTo(tarGdPtr->x, tarGdPtr->y);
	isInPath = 1;
}

void Robot::ToPutGoods()
{
	tarBerthId = SelectBerth();
	if (tarBerthId == -1)
	{
		isInPath = -1;
		return;
	}
	pair<int, int> pos = berth[tarBerthId].GetAvailablePos(x, y);
	MoveTo(pos.first, pos.second);
	isInPath = -1;
}

void Robot::Set(int id, int x, int y, int isCarrygoods, int status)
{
	this->id = id;
	this->x = x;
	this->y = y;
	if (isCarrygoods)
	{
		this->isInPath = -1;
	}
	this->status = status;
}
void Robot::FlushPos()
{
	Towards tw;
	if (curPathIndex < path.size())
	{
		tw = TwofNearPoint(path[curPathIndex].first, path[curPathIndex].second);
		if (Move(tw))
		{
			curPathIndex++;
		}
		else
		{
			curPathIndex--;
			tw = TwofNearPoint(path[curPathIndex].first, path[curPathIndex].second);
			if (!Move(tw))
			{
				curPathIndex++;
			}
		}
	}

	if (curPathIndex >= path.size())
	{
		if (isInPath == 1)
		{
			PickUp();
			ToPutGoods();
		}
		else if (isInPath == -1)
		{
			PutDown();
			ToGetGoods();
		}
	}
}
void Robot::FlushAction()
{
	if (isInPath == 1)
	{
		if (gdMap[tarGdPtr->id])
		{
			FlushPos();
		}
		else
		{
			isInPath = 0;
		}
	}
	else if (isInPath == -1)
	{
		FlushPos();
	}
	if (!isInPath)
	{
		ToGetGoods();
	}
}
void Robot::MoveTo(int x, int y)
{
	path.clear();
	curPathIndex = 0;

}
///////////////////////////////////////////////////////////////
Berth::Berth()
{
}

void Berth::Set(int id, int x, int y, int transport_time, int loading_speed)
{
	this->id = id;
	this->ltx = x;
	this->lty = y;
	this->rbx = x + 3;
	this->rby = y + 3;
	this->transport_time = transport_time;
	this->loading_speed = loading_speed;
}

pair<int, int> Berth::GetAvailablePos(int x, int y)
{
	pair<int, int> pos;
	//计算离机器人最近的可用位置
	if (x > rbx)
	{
		pos.first = rbx;
	}
	else if (x < ltx)
	{
		pos.first = ltx;
	}
	else
	{
		pos.first = x;
	}
	if (y > rby)
	{
		pos.second = rby;
	}
	else if (y < lty)
	{
		pos.second = lty;
	}
	else
	{
		pos.second = y;
	}
	return pos;
}

pair<int, int> Berth::GetGoods()
{
	pair<int, int> num_value(0, 0);
	int num = berthGoods.size();
	if (num < loading_speed)
	{
		num_value.first = num;
		for (int i = 0; i < num; i++)
		{
			num_value.second += berthGoods[i].val;
		}
		berthGoods.clear();
	}
	else
	{
		num_value.first = loading_speed;
		for (int i = 0; i < loading_speed; i++)
		{
			num_value.second += berthGoods[i].val;
		}
		berthGoods.erase(berthGoods.begin(), berthGoods.begin() + loading_speed);
	}
	totalGoodsValue -= num_value.second;
	return num_value;
}

void Berth::AddGoods(Goods* gdPtr)
{
	Goods gd = *gdPtr;
	berthGoods.push_back(gd);
	totalGoodsValue += gd.val;
}
int Berth::GetDistance(int x, int y)
{
	return abs(ltx - x) + abs(lty - y);
}
///////////////////////////////////////////////////////////////

int Boat::SelectBerth()
{
	int maxValue = INT_MIN;//性价比最大值	
	int maxId = -1;
	for (int i = 0; i < berth_num; i++)
	{
		if (!berth[i].isBoatComing && berth[i].boat_id == -1)
		{
			if (berth[i].totalGoodsValue / berth[i].transport_time > maxValue)
			{
				maxId = i;
			}
		}
	}
	return maxId;
}

bool Boat::IsAvailable()
{
	if (status == 1 && pos == -1)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void Boat::ShipTo(int berthId)
{
	cout << "ship " << id << " " << berthId << endl;
}

void Boat::GoToSell()
{
	cout << "go " << id << endl;
	//将船的货物数量和价值置为0
	goodsNum = 0;
	goodsValue = 0;
	//将泊位的船状态置为无船
	berth[pos].boat_id = -1;
}

bool Boat::IsOkToSell()
{
	if (goodsNum >= boat_capacity / 2)
	{
		return true;
	}
	if (goodsValue >= sell_price)
	{
		return true;
	}
	if (flushid - inBerthFlushId > 500)
	{
		return true;
	}
	return false;
}

void Boat::ToLoadGoods()
{
	pos = SelectBerth();
	if (pos == -1)
	{
		return;
	}
	//将泊位的船状态置为有船来
	berth[pos].isBoatComing = true;
	ShipTo(pos);
}

void Boat::LoadGoods()
{
	if (berth[pos].isBoatComing)
	{
		//入泊位操作
		berth[pos].isBoatComing = false;
		berth[pos].boat_id = id;
		inBerthFlushId = flushid;
	}
	pair<int, int> num_value = berth[pos].GetGoods();
	goodsNum += num_value.first;
	goodsValue += num_value.second;
}

Boat::Boat()
{
}

void Boat::FlushAction()
{
	//如果船不闲
	if (!IsAvailable())
	{
		//船在泊位上
		if (status == 1)
		{
			//装货
			LoadGoods();
			//如果船满足卖货条件
			if (IsOkToSell())
			{
				GoToSell();
			}
		}
		//船在运输中什么也不用做
		return;
	}
	//船闲的话安排船去泊位装货
	ToLoadGoods();
}

void Boat::Set(int id, int status, int pos)
{
	this->id = id;
	this->pos = pos;
	this->status = status;
}
///////////////////////////////////////////////////////////////
Goods::Goods(int id, int x, int y, int val, int birthflushid) :status(0)
{
	this->id = id;
	this->x = x;
	this->y = y;
	this->val = val;
	this->birthflushid = birthflushid;
}
bool Goods::IsAlive()
{
	return birthflushid + 1000 >= flushid;
}

///////////////////////////////////////////////////////////////
//初始化
void Manager::Init()
{
	for (int i = 0; i < N; i++)
		cin >> map[i];
	//读取泊位信息
	for (int i = 0; i < berth_num; i++)
	{
		int id;
		cin >> id;
		int ltx, lty, transport_time, loading_speed;
		cin >> ltx >> lty >> transport_time >> loading_speed;
		berth[id].Set(id, ltx, lty, transport_time, loading_speed);
	}
	//船的最大装货量
	cin >> boat_capacity;
	char okk[100];
	cin >> okk;
	cout << "OK\n";
	fflush(stdout);
}
//每帧输入读取
void Manager::Input()
{
	cin >> flushid >> money;
	int num;
	cin >> num;
	//读取新增货物信息
	for (int i = 0; i < num; i++)
	{
		int x, y, val;
		cin >> x >> y >> val;
		goods.push_back(Goods(total_goods, x, y, val, flushid));
		gdMap[total_goods] = true;
		total_goods++;
	}
	//读取机器人信息
	for (int i = 0; i < robot_num; i++)
	{
		int goods, x, y, sts;
		cin >> goods >> x >> y >> sts;
		robot[i].Set(i, x, y, goods, sts);
	}
	//读取船信息
	for (int i = 0; i < boat_num; i++)
	{
		int status, pos;
		cin >> status >> pos;
		boat[i].Set(i, status, pos);
	}
	char okk[100];
	cin >> okk;
}

void Manager::ClearDeadGoods()
{
	for (int i = 0; i < goods.size(); i++)
	{
		int gdId = goods[i].id;
		if (gdMap[gdId] && !goods[i].IsAlive())
		{
			goods.erase(goods.begin() + i);
			gdMap[gdId] = false;
			i--;
		}
	}
}

void Manager::FlushOperation()
{
	//清除死亡货物
	ClearDeadGoods();
	//机器人操作
	for (int i = 0; i < robot_num; i++)
	{
		robot[i].FlushAction();
	}
	//船操作
	for (int i = 0; i < boat_num; i++)
	{
		boat[i].FlushAction();
	}
}

void Manager::React()
{
	//每帧固定操作
	FlushOperation();
	//结束
	puts("OK");
	fflush(stdout);
}
