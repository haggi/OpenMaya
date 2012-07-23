#include "logging.h"

void Logging::setLogLevel( Logging::LogLevel level)
{
	if( level == Logging::Debug)
		std::cout << "Set logging level to DEBUG\n";
	if( level == Logging::Info)
		std::cout << "Set logging level to INFO\n";
	if( level == Logging::Warning)
		std::cout << "Set logging level to WARNING\n";
	if( level == Logging::Error)
		std::cout << "Set logging level to ERROR\n";
	if( level == Logging::Progress)
		std::cout << "Set logging level to PROGRESS\n";
	log_level = level;
}

void Logging::info(std::string logString)
{
	if( log_level >= Logging::Info )
		std::cout << "INFO: " << logString.c_str() << "\n";
}

void Logging::warning(std::string logString)
{
	if( log_level >= Logging::Warning )
		std::cout << "WARN: " << logString.c_str() << "\n";
}

void Logging::error(std::string logString)
{
	if( log_level >= Logging::Error )
		std::cerr << "ERROR: " << logString.c_str() << "\n";
}

void Logging::debug(std::string logString)
{
	if( log_level >= Logging::Debug )
		std::cout << "DEBUG: " << logString.c_str() << "\n";
}

void Logging::progress(std::string logString)
{
	if( log_level >= Logging::Progress )
		std::cout << "PROG: " << logString.c_str() << "\n";
}
