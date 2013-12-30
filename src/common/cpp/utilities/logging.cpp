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
	if( level == Logging::Detail)
		MGlobal::displayInfo("Set logging level to DETAIL");
	if( level == Logging::Feature)
		MGlobal::displayInfo("Set logging level to FEATURE");
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
	if( Logging::Info <= log_level )
		if( log_outtype == Logging::ScriptEditor)
			MGlobal::displayInfo(outString);
		else
			trace(outString);
}

void Logging::warning(MString logString, int level)
{
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB WARN: " + makeSpace(level) + logString;
	if( Logging::Warning <= log_level )
		if( log_outtype == Logging::ScriptEditor)
			MGlobal::displayInfo(outString);
		else
			trace(outString);
}

void Logging::error(MString logString, int level)
{
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB ERROR: " + makeSpace(level) + logString;
	if( Logging::Error <= log_level )
		if( log_outtype == Logging::ScriptEditor)
			MGlobal::displayInfo(outString);
		else
			trace(outString);
}

void Logging::debug(MString logString, int level)
{
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB DEBUG: " + makeSpace(level) + logString;
	if( Logging::Debug <= log_level )
		if( log_outtype == Logging::ScriptEditor)
			MGlobal::displayInfo(outString);
		else
			trace(outString);
}

void Logging::progress(MString logString, int level)
{
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB PROG: " + makeSpace(level) + logString;
	if( Logging::Progress <= log_level )
		if( log_outtype == Logging::ScriptEditor)
			MGlobal::displayInfo(outString);
		else
			trace(outString);
}

void Logging::detail(MString logString, int level)
{
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB DET: " + makeSpace(level) + logString;
	if( Logging::Detail <= log_level )
		if( log_outtype == Logging::ScriptEditor)
			MGlobal::displayInfo(outString);
		else
			trace(outString);
}

void Logging::feature(MString logString, int level)
{
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB FEA: " + makeSpace(level) + logString;
	if( log_level == Logging::Feature )
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
