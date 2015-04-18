#include "threads/queue.h"
#include "utilities/logging.h"
#include "memory/memoryInfo.h"
#include <maya/MGlobal.h>
#include <maya/MFileIO.h>
#include <stdio.h>

static EventQueue::concurrent_queue<EventQueue::Event> LogEventQueue;

EventQueue::concurrent_queue<EventQueue::Event> *theLogEventQueue()
{
	return &LogEventQueue;
}

inline void logEvent(MString m)
{
	EventQueue::Event e;
	e.message = m;
	theLogEventQueue()->push(e);
}

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

void Logging::trace(MString logString)
{
#if MAYA_API_VERSION >= 201500
	MStringArray strArray;
	logString.split('\n', strArray);
	for (uint i = 0; i < strArray.length(); i++)
	{
		logString = strArray[i];
		logString.substitute("\"", "\\\"");
		MString cmd = MString("trace ") + "\"" + logString + "\";";
		MGlobal::executeCommand(cmd);
	}
#else
	std::cout << "trace: " << logString.asChar() << "\n";
	std::cout.flush();
#endif
}

void Logging::info(MString logString, int level)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
	if (log_level >= Logging::Info)
	{
		//Logging::trace(outString);
		logEvent(outString);
		//std::cout << outString.asChar() << "\n";
		//std::cout.flush();
	}
}

void Logging::warning(MString logString, int level)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
	//Logging::trace(outString);
	logEvent(outString);
}

void Logging::error(MString logString, int level)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
	if (log_level >= Logging::Error)
	{
		//Logging::trace(outString);
		logEvent(outString);
		//std::cout << outString.asChar() << "\n";
		//std::cout.flush();
	}
}

void Logging::debug(MString logString, int level)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
	if (log_level >= Logging::Debug)
	{
		//Logging::trace(outString);
		logEvent(outString);
		//std::cout << outString.asChar() << "\n";
		//std::cout.flush();
	}
}

void Logging::progress(MString logString, int level)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
	if (log_level >= Logging::Progress)
	{
		//Logging::trace(outString);
		logEvent(outString);
		//std::cout << outString.asChar() << "\n";
		//std::cout.flush();
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

MCallbackId LoggingWorker::startId = 0;
bool LoggingWorker::terminateLoop = false;

void LoggingWorker::LoggingWorkerLoop(float time, float lastTime, void *userPtr)
{
	EventQueue::Event e;
	if (theLogEventQueue()->try_pop(e))
	{
		Logging::trace(e.message);
	}
}