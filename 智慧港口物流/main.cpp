#include "Statement.h"

int money;//当前金钱
int boat_capacity;//船的最大装货量,个数
int id;//当前帧id
char map[N][N];//港口地图
int total_goods = 0;//货物总数
vector<Goods> good;//货物信息

Robot robot[robot_num];
Berth berth[berth_num];
Boat boat[boat_num];

//初始化
void Init()
{
	for (int i = 0; i < N; i++)
		cin>>map[i];
	//读取泊位信息
	for (int i = 0; i < berth_num; i++)
	{
		int id;
		cin >> id;
		int ltx, lty, transport_time, loading_speed;
		cin >> ltx >> lty >> transport_time >> loading_speed;
		berth[id].Set(ltx, lty, transport_time, loading_speed);
	}
	cin>>boat_capacity;
	char okk[100];
	cin >> okk;
	cout<<"OK\n";
	fflush(stdout);
}
//每帧输入读取
int Input()
{
	cin >> id >> money;
	int num;
	cin >> num;
	//读取新增货物信息
	for (int i = 0; i < num; i++)
	{
		int x, y, val;
		cin >> x >> y >> val;
		good.push_back(Goods(total_goods, x, y, val));
		total_goods++;
	}
	//读取机器人信息
	for (int i = 0; i < robot_num; i++)
	{
		int goods, x, y, sts;
		cin >> goods >> x >> y >> sts;
		robot[i].Set(x, y, goods, sts);
	}
	//读取船信息
	for (int i = 0; i < boat_num; i++)
	{
		int status, pos;
		cin >> status >> pos;
		boat[i].Set(status, pos);
	}
	char okk[100];
	cin >> okk;
	return id;
}

int main()
{
	Init();
	for (int zhen = 1; zhen <= 15000; zhen++)
	{
		//读取当前帧信息
		int id = Input();
		//每帧输出
		for (int i = 0; i < robot_num; i++)
			cout << "move " << i <<' '<< rand() % 4 << '\n';
		puts("OK");
		fflush(stdout);
	}

	return 0;
}
