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
	return 0;
}

int Robot::SelectBerth()
{
	return 0;
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
	pair<int, int> pos = berth[tarBerthId].GetAvailablePos();
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

pair<int, int> Berth::GetAvailablePos()
{
	pair<int, int> pos;
	pos.first = rbx - 1;
	pos.second = rby - 1;
	return pos;
}

void Berth::AddGoods(Goods* gdPtr)
{
	Goods gd = *gdPtr;
	berthGoods.push_back(gd);
}
///////////////////////////////////////////////////////////////

int Boat::SelectBerth()
{
	return 0;
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
}

bool Boat::IsOkToSell()
{
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
	return birthflushid + 1000 > flushid;
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
