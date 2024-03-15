#include "Logger.h"
Logger Logger::instance;//实例
Logger::Logger()
{
	isConsoleOn = true;
}
#ifdef _WIN32
// Windows平台的代码
Logger::~Logger()
{
	instance << "---程序停止运行---" << endl;
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
	instance << "---程序开始运行---" << endl;
}
Logger& Logger::operator<<(const char* log_info)
{
	logfile << log_info;
	return *this;
}
Logger& Logger::operator<<(int log_info)
{
	logfile << log_info;
	return *this;
}
Logger& Logger::operator<<(string log_info)
{
	logfile << log_info;
	return *this;
}
Logger& Logger::operator<<(char log_info)
{
	logfile << log_info;
	return *this;
}
Logger& Logger::operator<<(double log_info)
{
	logfile << log_info;
	return *this;
}
Logger& Logger::operator<<(float log_info)
{
	logfile << log_info;
	return *this;
}
Logger& Logger::operator<<(bool log_info)
{
	logfile << log_info ? "true" : "false";
	return *this;
}
Logger& Logger::operator<<(ostream& (*log_info)(ostream&))
{
	logfile << log_info;
	logfile.flush();
	return *this;
}
#elif __linux__
// Linux平台的代码
Logger::~Logger()
{
	instance << "---程序停止运行---" << endl;
	logfile.close();
}

void Logger::Initialize(bool isRestart, bool isConsoleOn, string filename)
{
	this->isConsoleOn = isConsoleOn;
}
Logger& Logger::operator<<(const char* log_info)
{
	return *this;
}
Logger& Logger::operator<<(int log_info)
{
	return *this;
}
Logger& Logger::operator<<(string log_info)
{
	return *this;
}
Logger& Logger::operator<<(char log_info)
{
	return *this;
}
Logger& Logger::operator<<(double log_info)
{
	return *this;
}
Logger& Logger::operator<<(float log_info)
{
	return *this;
}
Logger& Logger::operator<<(bool log_info)
{
	return *this;
}
Logger& Logger::operator<<(ostream& (*log_info)(ostream&))
{
	return *this;
}
#else
#error "Unknown platform"
#endif