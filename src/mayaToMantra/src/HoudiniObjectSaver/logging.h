#ifndef MTM_LOGGING_H
#define MTM_LOGGING_H

#include <iostream>

class Logging
{
public:
	enum LogLevel{
	Error,
	Warning,
	Info,
	Progress,
	Debug
	};

	static void setLogLevel( Logging::LogLevel level);
	void info(std::string logString);
	void warning(std::string logString);
	void error(std::string logString);
	void debug(std::string logString);
	void progress(std::string logString);
};

static  Logging::LogLevel log_level = Logging::Info;


#endif
