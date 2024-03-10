#include "Statement.h"

//初始化
void Init()
{
	for (int i = 0; i < N; i++)
		scanf("%s", map[i]);
	//读取泊位信息
	for (int i = 0; i < berth_num; i++)
	{
		int id;

		scanf("%d", &id);
		int ltx, lty, transport_time, loading_speed;
		scanf("%d%d%d%d", &ltx, &lty, &transport_time, &loading_speed);
		berth[id].Set(ltx, lty, transport_time, loading_speed);
	}
	scanf("%d", &boat_capacity);
	char okk[100];
	scanf("%s", okk);
	printf("OK\n");
	fflush(stdout);
}
//每帧输入读取
int Input()
{
	scanf("%d%d", &id, &money);
	int num;
	scanf("%d", &num);
	//读取新增货物信息
	for (int i = 0; i < num; i++)
	{
		int x, y, val;
		scanf("%d%d%d", &x, &y, &val);
		good.push_back(Goods(total_goods, x, y, val));
		total_goods++;
	}
	//读取机器人信息
	for (int i = 0; i < robot_num; i++)
	{
		int goods, x, y, sts;
		scanf("%d%d%d%d", &goods, &x, &y, &sts);
		robot[i].Set(x, y, goods, sts);
	}
	//读取船信息
	for (int i = 0; i < boat_num; i++)
	{
		int status, pos;
		scanf("%d%d\n", &status, &pos);
		boat[i].Set(status, pos);
	}
	char okk[100];
	scanf("%s", okk);
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
			printf("move %d %d\n", i, rand() % 4);
		puts("OK");
		fflush(stdout);
	}

	return 0;
}
