#include "Logger.h"
Logger Logger::instance;//实例
Logger::Logger()
{
	isConsoleOn = true;
}

Logger::~Logger()
{
	Debug_Log("---程序停止运行---");
	logfile.close();
}

void Logger::Initialize(bool isRestart, bool isConsoleOn, string filename)
{
	if (isRestart)
	{
		//重新记录日志文件
		logfile.open(filename);
	}
	else
	{
		//追加到日志文件末尾
		logfile.open(filename, std::ios::app);
	}
	if (!logfile)
	{
		//日志文件加载失败
		exit(1);
	}
	this->isConsoleOn = isConsoleOn;
	Debug_Log("---程序开始运行---");
}

void Logger::Debug_Log(string log_info)
{
	//输出日志信息
	logfile << log_info << std::endl;
	//刷新缓冲区
	logfile.flush();
}