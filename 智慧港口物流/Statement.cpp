#include "Statement.h"

extern int money;//当前金钱
extern int boat_capacity;//船的最大装货量,个数
extern int flushid;//当前帧id
extern char map[N][N];//港口地图
extern int total_goods;//货物总数
extern vector<Goods> goods;//货物信息
extern unordered_map<int, bool> gdMap;//货物id与是否可被捡起的映射
extern int berth_num;//采用的泊位数量


extern Robot robot[robot_num];
extern Berth berth[real_berth_num];
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

Towards Robot::TwofNearPoint()
{
	Towards tw;
	if (path[curPathIndex].first > x)
	{
		tw = Down;
	}
	else if (path[curPathIndex].first < x)
	{
		tw = Up;
	}
	else if (path[curPathIndex].second > y)
	{
		tw = Right;
	}
	else if (path[curPathIndex].second < y)
	{
		tw = Left;
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
	goods.erase(std::remove_if(goods.begin(), goods.end(), [&](const Goods& g) { return g.id == tarGdPtr->id; }), goods.end());
	//将目标货物指针置空
	delete tarGdPtr;
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
			if (!IsGoodsAccessible(goods[i].id))//此货物不可到达
			{
				continue;
			}
			int distance = abs(goods[i].x - x) + abs(goods[i].y - y);//机器人与货物的距离
			if (distance != 0)
			{
				if (nearGoods.size() < 3)//当前可捡货物不足五件，直接添加至数组内
				{
					pair<int, int>a0;
					a0.first = i;
					a0.second = distance;
					nearGoods.push_back(a0);
				}
				else
				{
					//寻找三个值中距离最长的与当前货物距离进行对比
					int max = 0;
					for (int j = 1; j < 3; j++)
					{
						if (nearGoods[j].second > nearGoods[max].second)
						{
							max = j;
						}
					}
					if (nearGoods[max].second > distance)//若当前货物距离小于数组中最远的货物，则替换
					{
						nearGoods[max].first = i;
						nearGoods[max].second = distance;
					}
				}
			}

		}
	}
	if (nearGoods.size() == 0)//可选货物数量为0，返回空
	{
		return NULL;
	}
	int maxcostPerId = -1;//储存三个货物中性价比最高的货物下标
	double costPerformance0 = 0;//储存三个货物中最高的性价比（货物价值/距离的平方）
	for (int i = 0; i < nearGoods.size(); i++)
	{
		if (nearGoods[i].second == 0)
		{
			continue;
		}
		double costPerformance = goods[nearGoods[i].first].val / (double)nearGoods[i].second;//储存当前的货物的性价比
		if (costPerformance > costPerformance0)//若当前货物性价比高于原定货物性价比，则替换
		{
			costPerformance0 = costPerformance;
			maxcostPerId = nearGoods[i].first;
		}
	}
	if (maxcostPerId == -1)//可选货物数量为0，返回空
	{
		return NULL;
	}
	return &goods[maxcostPerId];//返回数组中性价比最高的货物的地址指针
}

int Robot::SelectBerth()
{
	int berthId = -1;
	int nearst = INT_MAX;
	for (int i = 0; i < berth_num; i++)
	{
		if (!IsBerthAccessible(i))
		{
			continue;
		}
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
	Goods* tptr = SelectGoods();//第一次选货
	if (tptr == NULL)
	{
		isInPath = 0;
		return;
	}
	while (!GetPath(tptr->x, tptr->y).size())//若所选货物无法规划处路径则执行循环
	{
		unaccessGoods.push_back(tptr->id);
		tptr = SelectGoods();//再次选货
		if (tptr == NULL)
		{
			isInPath = 0;
			return;
		}
	}
	tarGdPtr = new Goods(*tptr);
	if (MoveTo(tarGdPtr->x, tarGdPtr->y))
	{
		isInPath = 1;
	}
	else
	{
		isInPath = 0;
	}
}

void Robot::ToPutGoods()
{
	tarBerthId = SelectBerth();
	while (tarBerthId == -1)
	{
		berth_num++;
		tarBerthId = SelectBerth();
	}
	pair<int, int> pos = berth[tarBerthId].GetAvailablePos(x, y);
	if (MoveTo(pos.first, pos.second))
	{
		isInPath = -1;
	}
	else
	{
		isInPath = 0;
	}
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
void Robot::AddAccessibleBerth(int berthPos)
{
	accessibleBerth.push_back(berthPos);
}
void Robot::FlushPos()
{
	Towards tw;
	if (curPathIndex < path.size())
	{
		tw = TwofNearPoint();
		if (Move(tw))
		{
			DL << "前进成功" << endl;
			curPathIndex++;
		}
		else
		{
			//退无可退增加起点
			if (curPathIndex < 2)
			{
				DL << "退无可退新增起点" << endl;
				int diffx[] = { 0,0,1,-1 };
				int diffy[] = { 1,-1,0,0 };
				for (int i = 0; i < 4; i++)
				{
					int nx = x + diffx[i];
					int ny = y + diffy[i];
					bool isExist = false;
					for (int j = 0; j < path.size() && j < 9; j++)
					{
						if (nx == path[j].first && ny == path[j].second)
						{
							isExist = true;
							break;
						}
					}
					if (isExist)
					{
						continue;
					}
					if (map[nx][ny] != '#' && map[nx][ny] != '*')
					{
						auto newStartPos = pair<int, int>(nx, ny);
						DL << "自身当前位置:" << x << ' ' << y << " 新增起点:" << newStartPos.first << " " << newStartPos.second << endl;
						path.insert(path.begin(), newStartPos);
						curPathIndex++;
						break;
					}
				}
			}
			DL << "扩展起点后当前CurPathIndex:" << curPathIndex << endl;
			//后退一步
			if (curPathIndex >= 2)
			{
				curPathIndex -= 2;
				tw = TwofNearPoint();
				if (!Move(tw))
				{
					DL << "后退失败" << endl;
					curPathIndex += 2;
				}
				else
				{
					DL << "后退成功" << endl;
					curPathIndex++;
				}
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
bool Robot::IsBerthAccessible(int berthPos)
{
	for (int i = 0; i < accessibleBerth.size(); i++)
	{
		if (accessibleBerth[i] == berthPos)
		{
			return true;
		}
	}
	return false;
}
bool Robot::IsGoodsAccessible(int goodsId)
{
	for (int i = 0; i < unaccessGoods.size(); i++)
	{
		if (unaccessGoods[i] == goodsId)
		{
			return false;
		}
	}
	return true;
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
	unaccessGoods.clear();//该帧结束时清空机器人标记不可达货物数组
}
bool cmp(heapNode a, heapNode b)   //建立小顶堆
{
	if (a.F > b.F) return true;
	else return false;
}
bool isOutRange(int x, int y)
{
	return x >= 200 || x < 0 || y >= 200 || y < 0;
}
void change_F(int x, int y, int F, vector<heapNode>& path)
{
	for (int i = 0; i < path.size(); i++)
	{
		if (path[i].x == x && path[i].y == y)
		{
			path[i].F = F;
			break;
		}
	}
}
bool Robot::MoveTo(int x, int y)
{
	path.clear();
	curPathIndex = 1;


	auto temPath = GetPath(x, y);
	if (temPath.size() == 0)
	{
		return false;
	}
	else
	{
		path = temPath;
		return true;
	}
}
vector<pair<int, int>> Robot::GetPath(int x, int y)
{
	vector<pair<int, int>> tempPath;
	vector<heapNode> heap;
	int isPassed[200][200] = { 0 };     //未访问为0，在openlist为1，在closelist为2
	int G[200][200] = { 0 };   //从起点到该点所走步数
	int direction[200][200] = { 0 };   //来自哪个方向，1234：右左上下
	heapNode temp_heapNode = { this->x,this->y,abs(this->x - x) + abs(this->y - y) };
	heap.push_back(temp_heapNode);
	heapNode cur;

	bool flag = true;  //标记能否找到路径
	while (isPassed[x][y] == 0)  //访问终点后结束循环
	{

		//将堆顶结点弹出
		if (heap.size())
		{
			cur = *heap.begin();
			pop_heap(heap.begin(), heap.end(), cmp);
			heap.pop_back();
			isPassed[cur.x][cur.y] = 2;
		}
		else { flag = false; break; }
		//将堆顶结点可到达且未访问的结点压入堆,且更新已在堆中的结点信息
		//判断右位置
		if (!isOutRange(cur.x, cur.y + 1) && isPassed[cur.x][cur.y + 1] != 2 && map[cur.x][cur.y + 1] != '#' && map[cur.x][cur.y + 1] != '*')
		{
			if (isPassed[cur.x][cur.y + 1] == 0)
			{
				isPassed[cur.x][cur.y + 1] = 1;
				G[cur.x][cur.y + 1] = G[cur.x][cur.y] + 1;
				direction[cur.x][cur.y + 1] = 1;
				temp_heapNode.x = cur.x, temp_heapNode.y = cur.y + 1;
				temp_heapNode.F = G[cur.x][cur.y + 1] + abs(cur.x - x) + abs(cur.y + 1 - y);
				heap.push_back(temp_heapNode);
			}
			else {
				if (G[cur.x][cur.y + 1] > G[cur.x][cur.y] + 1)
				{
					change_F(cur.x, cur.y + 1, G[cur.x][cur.y] + 1 + abs(cur.x - x) + abs(cur.y + 1 - y), heap);
					direction[cur.x][cur.y + 1] = 1;
				}
			}
		}
		//判断左位置
		if (!isOutRange(cur.x, cur.y - 1) && isPassed[cur.x][cur.y - 1] != 2 && map[cur.x][cur.y - 1] != '#' && map[cur.x][cur.y - 1] != '*')
		{
			if (isPassed[cur.x][cur.y - 1] == 0)
			{
				isPassed[cur.x][cur.y - 1] = 1;
				G[cur.x][cur.y - 1] = G[cur.x][cur.y] + 1;
				direction[cur.x][cur.y - 1] = 2;
				temp_heapNode.x = cur.x, temp_heapNode.y = cur.y - 1;
				temp_heapNode.F = G[cur.x][cur.y - 1] + abs(cur.x - x) + abs(cur.y - 1 - y);
				heap.push_back(temp_heapNode);
			}
			else {
				if (G[cur.x][cur.y - 1] > G[cur.x][cur.y] + 1)
				{
					change_F(cur.x, cur.y - 1, G[cur.x][cur.y] + 1 + abs(cur.x - x) + abs(cur.y - 1 - y), heap);
					direction[cur.x][cur.y - 1] = 2;
				}
			}
		}
		//判断上位置
		if (!isOutRange(cur.x - 1, cur.y) && isPassed[cur.x - 1][cur.y] != 2 && map[cur.x - 1][cur.y] != '#' && map[cur.x - 1][cur.y] != '*')
		{
			if (isPassed[cur.x - 1][cur.y] == 0)
			{
				isPassed[cur.x - 1][cur.y] = 1;
				G[cur.x - 1][cur.y] = G[cur.x][cur.y] + 1;
				direction[cur.x - 1][cur.y] = 3;
				temp_heapNode.x = cur.x - 1, temp_heapNode.y = cur.y;
				temp_heapNode.F = G[cur.x - 1][cur.y] + abs(cur.x - 1 - x) + abs(cur.y - y);
				heap.push_back(temp_heapNode);
			}
			else {
				if (G[cur.x - 1][cur.y] > G[cur.x][cur.y] + 1)
				{
					change_F(cur.x - 1, cur.y, G[cur.x][cur.y] + 1 + abs(cur.x - 1 - x) + abs(cur.y - y), heap);
					direction[cur.x - 1][cur.y] = 3;
				}
			}
		}
		//判断下位置
		if (!isOutRange(cur.x + 1, cur.y) && isPassed[cur.x + 1][cur.y] != 2 && map[cur.x + 1][cur.y] != '#' && map[cur.x + 1][cur.y] != '*')
		{
			if (isPassed[cur.x + 1][cur.y] == 0)
			{
				isPassed[cur.x + 1][cur.y] = 1;
				G[cur.x + 1][cur.y] = G[cur.x][cur.y] + 1;
				direction[cur.x + 1][cur.y] = 4;
				temp_heapNode.x = cur.x + 1, temp_heapNode.y = cur.y;
				temp_heapNode.F = G[cur.x + 1][cur.y] + abs(cur.x + 1 - x) + abs(cur.y - y);
				heap.push_back(temp_heapNode);
			}
			else {
				if (G[cur.x + 1][cur.y] > G[cur.x][cur.y] + 1)
				{
					change_F(cur.x + 1, cur.y, G[cur.x][cur.y] + 1 + abs(cur.x + 1 - x) + abs(cur.y - y), heap);
					direction[cur.x + 1][cur.y] = 4;
				}
			}
		}
		push_heap(heap.begin(), heap.end(), cmp);   //建立小顶堆
	}

	int cur_x = x, cur_y = y;
	if (flag)   //如果可找到路径
	{
		while (cur_x != this->x || cur_y != this->y)   //当回溯到起点时结束循环
		{
			tempPath.push_back(pair<int, int>(cur_x, cur_y));
			switch (direction[cur_x][cur_y])
			{
			case 1:cur_y -= 1; break;
			case 2:cur_y += 1; break;
			case 3:cur_x += 1; break;
			case 4:cur_x -= 1; break;
			};
		}
		tempPath.push_back(pair<int, int>(this->x, this->y));
		reverse(tempPath.begin(), tempPath.end());
		return tempPath;    //找到路径返回tr
	}
	return vector<pair<int, int>>();     //找不到路径返回
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
int Berth::GetDistance(int x, int y)
{
	return abs(ltx - x) + abs(lty - y);
}
void Berth::AddGoods(Goods* gdPtr)
{
	Goods gd = *gdPtr;
	berthGoods.push_back(gd);
	totalGoodsValue += gd.val;
}
///////////////////////////////////////////////////////////////

int Boat::SelectBerth()
{
	float maxValue = INT_MIN;//性价比最大值	
	int maxIdex = -1;
	for (int i = 0; i < berth_num; i++)
	{
		if (!berth[i].isBoatComing && berth[i].boat_id == -1)
		{
			float cv = berth[i].totalGoodsValue / (float)berth[i].transport_time;
			if (cv > maxValue)
			{
				maxValue = cv;
				maxIdex = i;
			}
		}
	}
	return maxIdex;
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
	pos = -1;
}

bool Boat::IsOkToSell()
{
	if (goodsNum >= boat_capacity)
	{
		return true;
	}
	if (flushid - inBerthFlushId > 1000 && goodsNum < 4)
	{
		return true;
	}
	if (15000 - flushid <= berth[pos].transport_time + 5)
	{
		return true;
	}
	return false;
}

void Boat::ToLoadGoods()
{
	pos = SelectBerth();
	//pos = id;
	if (pos == -1)
	{
		return;
	}
	//将泊位的船状态置为有船来
	berth[pos].isBoatComing = true;
	ShipTo(berth[pos].id);
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
	//this->pos = pos;
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

bool Goods::operator==(const Goods& gd)
{
	if (id == gd.id)
	{
		return true;
	}
	return false;
}


///////////////////////////////////////////////////////////////
//初始化
bool Manager::isAccessible(int x, int y)
{
	queue<pair<int, int>> q;

	bool visited[N][N] = { false };

	visited[x][y] = true;
	q.push({ x, y });

	int dx[] = { -1, 1, 0, 0 };
	int dy[] = { 0, 0, -1, 1 };

	while (!q.empty())
	{
		pair<int, int> curr = q.front();
		q.pop();

		for (int i = 0; i < 4; i++)
		{
			int nx = curr.first + dx[i];
			int ny = curr.second + dy[i];
			if (map[nx][ny] == 'B')
			{
				return true;
			}
			if (nx >= 0 && nx < N && ny >= 0 && ny < N && !visited[nx][ny] && (map[nx][ny] == '.' || map[nx][ny] == 'A' || isdigit(map[nx][ny])))
			{
				visited[nx][ny] = true;
				q.push({ nx, ny });
			}
		}
	}
	return false;
}
void Manager::markAccessibleRobot(int x, int y, int berthPos)//表示更改的加可到达泊位属性
{
	queue<pair<int, int>> q;

	bool visited[N][N] = { false };

	visited[x][y] = true;
	q.push({ x, y });

	int dx[] = { -1, 1, 0, 0 };
	int dy[] = { 0, 0, -1, 1 };

	while (!q.empty())
	{
		pair<int, int> curr = q.front();
		q.pop();
		if (map[curr.first][curr.second] == 'A')
		{
			//将机器人所在位置的坐标存于泊位的可到达机器人数组里
			int r = 0;//标志位，若所到达机器人已经在vector容器里面，则赋值为1
			for (int i = 0; i < RobotXY.size(); i++)
			{
				if (RobotXY[i].first == curr.first && RobotXY[i].second == curr.second)//所到达机器人已经在vector容器里面
				{
					AccessBerthPos[i].push_back(berthPos);
					r = 1;
					break;
				}
			}
			if (r == 0)//所到达机器人还未被到达过
			{
				RobotXY.push_back({ curr.first ,curr.second });
				vector<int>a0;
				a0.push_back(berthPos);
				AccessBerthPos.push_back(a0);
			}
		}
		for (int i = 0; i < 4; i++)
		{
			int nx = curr.first + dx[i];
			int ny = curr.second + dy[i];
			if (nx >= 0 && nx < N && ny >= 0 && ny < N && !visited[nx][ny] && (map[nx][ny] == '.' || map[nx][ny] == 'A' || map[nx][ny] == 'B'))
			{
				visited[nx][ny] = true;
				q.push({ nx, ny });
			}
		}
	}
}
bool cmp2(const Berth& a, const Berth& b)
{
	return a.transport_time < b.transport_time;
}
void Manager::Init()
{
	for (int i = 0; i < N; i++)
		cin >> map[i];
	//读取泊位信息
	for (int i = 0; i < real_berth_num; i++)
	{
		int id;
		cin >> id;
		int ltx, lty, transport_time, loading_speed;
		cin >> ltx >> lty >> transport_time >> loading_speed;
		berth[i].Set(id, ltx, lty, transport_time, loading_speed);
	}
	sort(berth, berth + real_berth_num, cmp2);
	for (int i = 0; i < real_berth_num; i++)
	{
		markAccessibleRobot(berth[i].ltx, berth[i].lty, i);
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
		if (!isAccessible(x, y))
		{
			continue;
		}
		goods.push_back(Goods(total_goods, x, y, val, flushid));
		gdMap[total_goods] = true;
		total_goods++;
	}
	//读取机器人信息
	for (int i = 0; i < robot_num; i++)
	{
		int goods, x, y, sts;
		cin >> goods >> x >> y >> sts;
		if (flushid == 1)
		{
			int j;
			for (j = 0; j < RobotXY.size(); j++)
			{
				if (x == RobotXY[j].first && y == RobotXY[j].second)
				{
					for (int k = 0; k < AccessBerthPos[j].size(); k++)
					{
						robot[i].AddAccessibleBerth(AccessBerthPos[j][k]);
					}
					break;
				}
			}
			if (j == RobotXY.size())
			{
				robot[i].isAccesible = false;
			}
		}
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
	DL << "--------------------------------------------------------------------第" << flushid << "帧" << endl;
	//清除死亡货物
	ClearDeadGoods();
	//机器人操作
	for (int i = 0; i < robot_num; i++)
	{
		if (robot[i].isAccesible)
		{
			DL << "----------------------------------机器人" << i << "操作" << endl;
			robot[i].FlushAction();
		}
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
