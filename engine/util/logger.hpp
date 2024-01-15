/**
 * @file engine/util/logger.hpp
 * @author Caleb Burke
 * @date Jan 8, 2024
 *
 * Lots of work still needs to be done in here.
 *
 * TODO Create logger singleton class
 * TODO Impl log queuing and dequeuing
 * TODO Impl some form of log error handling
 */

#pragma once

#include <iostream>
#include <string>

namespace hep {
namespace logger {

enum class LEVEL {
	FATAL,
	ERROR,
	WARNING,
	INFO,
	VERBOSE,
	DEBUG,
	TRACE,
};

// Helper print functions
#define LOG_PRINT_HELPER(MSG)	std::cout << MSG
#define LOG_PREFIX_FATAL		LOG_PRINT_HELPER("\033[1;91m[ FATAL ]\033[0m ")
#define LOG_PREFIX_ERROR		LOG_PRINT_HELPER("\033[1;31m[ ERROR ]\033[0m ")
#define LOG_PREFIX_WARNING		LOG_PRINT_HELPER("\033[1;33m[ WARNING ]\033[0m ")
#define LOG_PREFIX_INFO 		LOG_PRINT_HELPER("\033[1;34m[ INFO ]\033[0m ")
#define LOG_PREFIX_VERBOSE		LOG_PRINT_HELPER("verbose")
#define LOG_PREFIX_DEBUG		LOG_PRINT_HELPER("\033[1;35m[ DEBUG ]\033[0m ")
#define LOG_PREFIX_TRACE 		LOG_PRINT_HELPER("\033[1;36m[ TRACE ]\033[0m ")

template<typename... Args>
void log(LEVEL level, Args... args){
	switch(level){
		case LEVEL::FATAL:
			LOG_PREFIX_FATAL;
			break;
		case LEVEL::ERROR:
			LOG_PREFIX_ERROR;
			break;
		case LEVEL::WARNING:
			LOG_PREFIX_WARNING;
			break;
		case LEVEL::INFO:
			LOG_PREFIX_INFO;
			break;
		case LEVEL::DEBUG:
			LOG_PREFIX_DEBUG;
			break;
		case LEVEL::TRACE:
			LOG_PREFIX_TRACE;
			break;
		default:
			break;
	}

	((std::cout << ' ' << std::forward<Args>(args)), ...);
	std::cout << std::endl;
}

}	// namespace logger

using logger::log;
using logger::LEVEL;

}	// namespace hep

