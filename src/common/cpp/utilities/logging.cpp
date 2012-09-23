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


void Logging::trace(MString logString)
{
	std::cout << "trace: " << logString << "\n";
	std::cout.flush();
}

void Logging::info(MString logString)
{
	if( log_level >= Logging::Info )
		MGlobal::displayInfo(MString("Mem: ") + getCurrentUsage() + "MB INFO: " + logString);
}

void Logging::warning(MString logString)
{
	if( log_level >= Logging::Warning )
		MGlobal::displayInfo(MString("Mem: ") + getCurrentUsage() + "MB WARN: " + logString);
}

void Logging::error(MString logString)
{
	if( log_level >= Logging::Error )
		MGlobal::displayInfo(MString("Mem: ") + getCurrentUsage() + "MB ERROR: " + logString);
}

void Logging::debug(MString logString)
{
	if( log_level >= Logging::Debug )
		MGlobal::displayInfo(MString("Mem: ") + getCurrentUsage() + "MB DEBUG: " + logString);
}

void Logging::progress(MString logString)
{
	if( log_level >= Logging::Progress )
		MGlobal::displayInfo(MString("Mem: ") + getCurrentUsage() + "MB PROG: " + logString);
}
