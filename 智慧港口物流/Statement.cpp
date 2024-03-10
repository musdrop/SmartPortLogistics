#include "Statement.h"

extern int money;//当前金钱
extern int boat_capacity;//船的最大装货量,个数
extern int id;//当前帧id
extern char map[N][N];//港口地图
extern int total_goods;//货物总数
extern vector<Goods> goods;//货物信息
extern unordered_map<int, bool> gdMap;//货物id与是否可被捡起的映射

extern Robot robot[robot_num];
extern Berth berth[berth_num];
extern Boat boat[boat_num];

bool Robot::Move(Towards tw)
{
	map[x][y] = preGround;
	switch (tw)
	{
	case Right:
		x++;
		break;
	case Left:
		x--;
		break;
	case Up:
		y--;
		break;
	case Down:
		y++;
		break;
	}
	if (map[x][y] == 'B' || map[x][y] == '.')
	{
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
	//从待选货物列表中移除捡起的货物
	int i = tarGdId > goods.size() - 1 ? goods.size() - 1 : tarGdId;
	for (; i >= 0; i--)
	{
		if (goods[i].id == tarGdId)
		{
			goods.erase(goods.begin() + i);
			gdMap[tarGdId] = false;
			break;
		}
	}
}

void Robot::PutDown()
{
	cout << "pull " << id << endl;
}

int Robot::SelectGoods()
{
	return 0;
}

int Robot::SelectBerth()
{
	return 0;
}

bool Robot::TargetGoods()
{
	return false;
}

Robot::Robot()
{
}

void Robot::ToGetGoods()
{
	tarGdId = SelectGoods();
	if (tarGdId == -1)
	{
		isInPath = 0;
		return;
	}
	MoveTo(goods[tarGdId].x, goods[tarGdId].y);
	isInPath = 1;
}

void Robot::ToPutGoods()
{
	int brthi = SelectBerth();
	if (brthi == -1)
	{
		isInPath = 0;
		return;
	}
	pair<int, int> pos = berth[brthi].GetAvailablePos();
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
void Robot::SetGoodsState(int id, bool state)
{
	gdMap[id] = state;
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
	return pair<int, int>();
}

///////////////////////////////////////////////////////////////
Boat::Boat()
{
}

void Boat::Set(int id, int status, int pos)
{
	this->id = id;
	this->pos = pos;
	this->status = status;
}
///////////////////////////////////////////////////////////////
Goods::Goods(int id, int x, int y, int val) :status(0), surtime(1000)
{
	this->id = id;
	this->x = x;
	this->y = y;
	this->val = val;
}
void Goods::LiveDown()
{
	surtime--;
}

bool Goods::IsAlive()
{
	return surtime > 0;
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
	cin >> boat_capacity;
	char okk[100];
	cin >> okk;
	cout << "OK\n";
	fflush(stdout);
}
//每帧输入读取
int Manager::Input()
{
	cin >> id >> money;
	int num;
	cin >> num;
	//读取新增货物信息
	for (int i = 0; i < num; i++)
	{
		int x, y, val;
		cin >> x >> y >> val;
		goods.push_back(Goods(total_goods, x, y, val));
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
	return id;
}

void Manager::ClearDeadGoods()
{
	for (int i = 0; i < goods.size(); i++)
	{
		goods[i].LiveDown();
		if (!goods[i].IsAlive())
		{
			goods.erase(goods.begin() + i);
			gdMap[i] = false;
			i--;
		}
	}
}

void Manager::FlushOperation()
{
	for (int i = 0; i < robot_num; i++)
	{
		if (robot[i].isInPath == 1)
		{
			if (gdMap[robot[i].tarGdId])
			{
				robot[i].FlushPos();
			}
			else
			{
				robot[i].isInPath = 0;
			}
		}
		else if (robot[i].isInPath == -1)
		{
			robot[i].FlushPos();
		}


		if (!robot[i].isInPath)
		{
			robot[i].ToGetGoods();
		}
	}

}

void Manager::React()
{
	//清除死亡货物
	ClearDeadGoods();
	//每帧固定操作
	FlushOperation();
	//结束
	puts("OK");
	fflush(stdout);
}
