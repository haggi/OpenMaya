#ifndef MTM_LOGGING_H
#define MTM_LOGGING_H

#include <maya/MString.h>
#include <maya/MTimerMessage.h>
#include <thread>
#include <maya/MStreamUtils.h>

#define COUT(msg) MStreamUtils::stdOutStream() << msg << "\n"

class Logging
{
public:
	enum LogLevel{
		Info = 0,
		Error = 1,
		Warning = 2,
		Progress = 3,
		Debug = 4,
		None = 5
	};
	enum OutputType{
		ScriptEditor,
		OutputWindow
	};

	static void setLogLevel( Logging::LogLevel level);
	static void info(MString logString);
	static void warning(MString logString);
	static void error(MString logString);
	static void debug(MString logString);
	static void debugs(MString logString, int level);
	static void progress(MString logString);
	static void detail(MString logString);
};

MString makeSpace(int level);
static  Logging::LogLevel log_level = Logging::Info;
static  Logging::OutputType log_outtype = Logging::ScriptEditor;

#endif
