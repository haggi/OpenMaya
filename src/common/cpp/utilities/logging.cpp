#include "threads/queue.h"
#include "utilities/logging.h"
#include "memory/memoryInfo.h"
#include <maya/MGlobal.h>
#include <stdio.h>
#include <thread>

void Logging::setLogLevel( Logging::LogLevel level)
{
	if (level == Logging::Debug)
	{
		MGlobal::displayInfo("Set logging level to DEBUG");
	}
	if (level == Logging::Info)
	{
		MGlobal::displayInfo("Set logging level to INFO");
	}
	if (level == Logging::Warning)
	{
		MGlobal::displayInfo("Set logging level to WARNING");
	}
	if (level == Logging::Error)
	{
		MGlobal::displayInfo("Set logging level to ERROR");
	}
	if (level == Logging::Progress)
	{
		MGlobal::displayInfo("Set logging level to PROGRESS");
	}
	log_level = level;
}

void Logging::info(MString logString)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " +  logString;
	if (log_level >= Logging::Info)
		COUT(outString);
}

void Logging::warning(MString logString)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " +  logString;
	if (log_level >= Logging::Warning)
		COUT(outString);
}

void Logging::error(MString logString)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " +  logString;
	if (log_level >= Logging::Error)
	{
		COUT(outString);
	}
}

void Logging::debug(MString logString)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " +  logString;
	if (log_level >= Logging::Debug)
	{
		COUT(outString);
	}
}

void Logging::debugs(MString logString, int level)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
	if (log_level >= Logging::Debug)
	{
		COUT(outString);
	}
}

void Logging::progress(MString logString)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " +  logString;
	if (log_level >= Logging::Progress)
	{
		COUT(outString);
	}
}

void Logging::detail(MString logString)
{
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " +  logString;
	COUT(outString);
}

MString makeSpace(int level)
{
	MString space;
	for( int i = 0; i < level; i++)
		space += "\t";
	return space;
}
