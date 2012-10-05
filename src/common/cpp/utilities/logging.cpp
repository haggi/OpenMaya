#include "utilities/logging.h"
#include "memory/memoryInfo.h"
#include <maya/MGlobal.h>

void Logging::setLogLevel( Logging::LogLevel level)
{
	if( level == Logging::Debug)
		MGlobal::displayInfo("Set logging level to DEBUG");
	if( level == Logging::Info)
		MGlobal::displayInfo("Set logging level to INFO");
	if( level == Logging::Warning)
		MGlobal::displayInfo("Set logging level to WARNING");
	if( level == Logging::Error)
		MGlobal::displayInfo("Set logging level to ERROR");
	if( level == Logging::Progress)
		MGlobal::displayInfo("Set logging level to PROGRESS");
	log_level = level;
}

void Logging::setOutType(Logging::OutputType outtype)
{
	log_outtype = outtype;
}

void Logging::trace(MString logString, int level)
{
	std::cout << "trace: " << makeSpace(level) + logString << "\n";
	std::cout.flush();
}

void Logging::info(MString logString, int level)
{
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB INFO: " + makeSpace(level) + logString;
	if( log_level >= Logging::Info )
		if( log_outtype == Logging::ScriptEditor)
			MGlobal::displayInfo(outString);
		else
			trace(outString);
}

void Logging::warning(MString logString, int level)
{
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB WARN: " + makeSpace(level) + logString;
	if( log_level >= Logging::Warning )
		if( log_outtype == Logging::ScriptEditor)
			MGlobal::displayInfo(outString);
		else
			trace(outString);
}

void Logging::error(MString logString, int level)
{
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB ERROR: " + makeSpace(level) + logString;
	if( log_level >= Logging::Error )
		if( log_outtype == Logging::ScriptEditor)
			MGlobal::displayInfo(outString);
		else
			trace(outString);
}

void Logging::debug(MString logString, int level)
{
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB DEBUG: " + makeSpace(level) + logString;
	if( log_level >= Logging::Debug )
		if( log_outtype == Logging::ScriptEditor)
			MGlobal::displayInfo(outString);
		else
			trace(outString);
}

void Logging::progress(MString logString, int level)
{
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB PROG: " + makeSpace(level) + logString;
	if( log_level >= Logging::Progress )
		if( log_outtype == Logging::ScriptEditor)
			MGlobal::displayInfo(outString);
		else
			trace(outString);
}

MString makeSpace(int level)
{
	MString space;
	for( int i = 0; i < level; i++)
		space += "\t";
	return space;
}
