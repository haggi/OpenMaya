#ifndef MTM_LOGGING_H
#define MTM_LOGGING_H

#include <maya/MString.h>
#include <maya/MTimerMessage.h>

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
	static void setOutType( Logging::OutputType outtype);
	static void info(MString logString, int level = 0);
	static void warning(MString logString, int level = 0);
	static void error(MString logString, int level = 0);
	static void debug(MString logString, int level = 0);
	static void progress(MString logString, int level = 0);
	static void detail(MString logString, int level = 0);
	static void feature(MString logString, int level = 0);
	static void trace(MString logString);
};

MString makeSpace(int level);
static  Logging::LogLevel log_level = Logging::Info;
static  Logging::OutputType log_outtype = Logging::ScriptEditor;


class LoggingWorker{
public:
	static MCallbackId startId;
	static bool terminateLoop;
	static void LoggingWorkerLoop(float time, float lastTime, void *userPtr);
};

#endif
