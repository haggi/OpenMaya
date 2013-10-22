#ifndef MTM_LOGGING_H
#define MTM_LOGGING_H

#include <maya/MString.h>

class Logging
{
public:
	enum LogLevel{
	Error,
	Warning,
	Info,
	Progress,
	Debug,
	Detail,
	Feature
	};
	enum OutputType{
	ScriptEditor,
	OutputWindow
	};

	static void setLogLevel( Logging::LogLevel level);
	static void setOutType( Logging::OutputType outtype);
	void info(MString logString, int level = 0);
	void warning(MString logString, int level = 0);
	void error(MString logString, int level = 0);
	void debug(MString logString, int level = 0);
	void progress(MString logString, int level = 0);
	void detail(MString logString, int level = 0);
	void feature(MString logString, int level = 0);
	void trace(MString logString, int level = 0);
};

MString makeSpace(int level);
static  Logging::LogLevel log_level = Logging::Info;
static  Logging::OutputType log_outtype = Logging::ScriptEditor;


#endif
