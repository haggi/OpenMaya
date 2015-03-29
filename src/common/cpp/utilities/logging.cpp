#include "utilities/logging.h"
#include "memory/memoryInfo.h"
#include <maya/MGlobal.h>

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
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
	if (log_level >= Logging::Info)
	{
		std::cout << outString.asChar() << "\n";
		std::cout.flush();
	}
}

void Logging::warning(MString logString, int level)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
}

void Logging::error(MString logString, int level)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
	if (log_level >= Logging::Error)
	{
		std::cout << outString.asChar() << "\n";
		std::cout.flush();
	}
}

void Logging::debug(MString logString, int level)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
	if (log_level >= Logging::Debug)
	{
		std::cout << outString.asChar() << "\n";
		std::cout.flush();
	}
}

void Logging::progress(MString logString, int level)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
	if (log_level >= Logging::Progress)
	{
		std::cout << outString.asChar() << "\n";
		std::cout.flush();
	}
}

void Logging::detail(MString logString, int level)
{
}

void Logging::feature(MString logString, int level)
{
}

MString makeSpace(int level)
{
	MString space;
	for( int i = 0; i < level; i++)
		space += "\t";
	return space;
}
