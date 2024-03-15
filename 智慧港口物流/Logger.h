#pragma once
#include <fstream>
#include <string>
using namespace std;
class Logger
{
public:
	static Logger instance;//实例
private:
	ofstream logfile;//日志文件流对象
	bool isConsoleOn;
public:
	Logger();
	~Logger();
	//运行程序时在主函数入口出执行(建议执行：Logger::instance.Initialize(true);）
	void Initialize(bool isRestart, bool isConsoleOn = true, string filename = "LogMessage.txt");//日志初始化
	Logger& operator<<(const char* log_info);
	Logger& operator<<(int log_info);
	Logger& operator<<(string log_info);
	Logger& operator<<(char log_info);
	Logger& operator<<(double log_info);
	Logger& operator<<(float log_info);
	Logger& operator<<(bool log_info);
	Logger& operator<<(ostream& (*log_info)(ostream&));
};

