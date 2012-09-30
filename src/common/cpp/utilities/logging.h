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
	Debug
	};
	enum OutputType{
	ScriptEditor,
	OutputWindow
	};

	static void setLogLevel( Logging::LogLevel level);
	static void setOutType( Logging::OutputType outtype);
	void info(MString logString);
	void warning(MString logString);
	void error(MString logString);
	void debug(MString logString);
	void progress(MString logString);
	void trace(MString logString);
};

static  Logging::LogLevel log_level = Logging::Info;
static  Logging::OutputType log_outtype = Logging::ScriptEditor;


#endif
