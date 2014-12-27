#include "utilities/logging.h"
#include "memory/memoryInfo.h"
#include <maya/MGlobal.h>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>



void Logging::setLogLevel( Logging::LogLevel level)
{
	//boost::log::add_common_attributes();
	//boost::log::core::get_global_attributes()
	//boost::log::core::get()->remove_global_attribute();

	if (level == Logging::Debug)
	{
		MGlobal::displayInfo("Set logging level to DEBUG");
		boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::debug);
		//boost::log::core::set_global_attributes();
	}
	if (level == Logging::Info)
	{
		MGlobal::displayInfo("Set logging level to INFO");
		boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::info);
	}
	if (level == Logging::Warning)
	{
		MGlobal::displayInfo("Set logging level to WARNING");
		boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::warning);
	}
	if (level == Logging::Error)
	{
		MGlobal::displayInfo("Set logging level to ERROR");
		boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::error);
	}
	if (level == Logging::Progress)
	{
		MGlobal::displayInfo("Set logging level to PROGRESS");
		boost::log::core::get()->set_filter(boost::log::trivial::severity >= boost::log::trivial::trace);
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
	MString outString = MString("Mem: ") + (int)getCurrentUsage() + "MB : " + makeSpace(level) + logString;
	BOOST_LOG_TRIVIAL(info) << outString.asChar();
}

void Logging::warning(MString logString, int level)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
	BOOST_LOG_TRIVIAL(warning) << outString.asChar();
}

void Logging::error(MString logString, int level)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
	BOOST_LOG_TRIVIAL(error) << outString.asChar();
}

void Logging::debug(MString logString, int level)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
	BOOST_LOG_TRIVIAL(debug) << outString.asChar();
}

void Logging::progress(MString logString, int level)
{
	if (log_level == 5)
		return;
	MString outString = MString("Mem: ") + getCurrentUsage() + "MB " + makeSpace(level) + logString;
	BOOST_LOG_TRIVIAL(trace) << outString.asChar();
}

void Logging::detail(MString logString, int level)
{
	//MString outString = MString("Mem: ") + getCurrentUsage() + "MB DET: " + makeSpace(level) + logString;
	//if( Logging::Detail <= log_level && log_level != Logging::Feature )
	//	if( log_outtype == Logging::ScriptEditor)
	//		MGlobal::displayInfo(outString);
	//	else
	//		trace(outString);
}

void Logging::feature(MString logString, int level)
{
	//MString outString = MString("Mem: ") + getCurrentUsage() + "MB FEA: " + makeSpace(level) + logString;
	//if( log_level == Logging::Feature )
	//	if( log_outtype == Logging::ScriptEditor)
	//		MGlobal::displayInfo(outString);
	//	else
	//		trace(outString);
}

MString makeSpace(int level)
{
	MString space;
	for( int i = 0; i < level; i++)
		space += "\t";
	return space;
}
