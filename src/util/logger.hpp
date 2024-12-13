/**
 * @file src/util/logger.hpp
 * @author Caleb Burke
 * @date Jan 8, 2024
 *
 * Lots of work still needs to be done in here.
 *
 * Usage:
 * using namespace hep;
 * log::fatal("fatal message")
 * log::info("Infomation message")
 *
 * TODO Impl some form of log error handling
 */

#pragma once

#include "types.hpp"
#include <iostream>
#include <utility>

namespace hep {
namespace log {

enum class LEVEL {
	FATAL,
	ERROR,
	WARNING,
	INFO,
	VERBOSE,
	DEBUGGING,
	TRACE,
};

// Helper print functions
#define LOG_PRINT_HELPER(MSG)	std::cout << MSG
#define LOG_PREFIX_FATAL		LOG_PRINT_HELPER("\033[1;91m[FATAL]\033[0m ")
#define LOG_PREFIX_ERROR		LOG_PRINT_HELPER("\033[1;31m[ERROR]\033[0m ")
#define LOG_PREFIX_WARNING		LOG_PRINT_HELPER("\033[1;33m[WARNING]\033[0m ")
#define LOG_PREFIX_INFO 		LOG_PRINT_HELPER("\033[1;34m[INFO]\033[0m ")
#define LOG_PREFIX_VERBOSE		LOG_PRINT_HELPER("\033[1;33m[VERBOSE]\033[0m ")
#define LOG_PREFIX_DEBUG		LOG_PRINT_HELPER("\033[1;35m[DEBUG]\033[0m ")
#define LOG_PREFIX_TRACE 		LOG_PRINT_HELPER("\033[1;36m[TRACE]\033[0m ")

template<typename... Args>
void log(LEVEL level, Args... args){
#ifndef NDEBUG
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
        case LEVEL::VERBOSE:
            LOG_PREFIX_VERBOSE;
            break;
        //case LEVEL::DEBUGGING:
        //    LOG_PREFIX_DEBUG;
        //    break;
        case LEVEL::TRACE:
            LOG_PREFIX_TRACE;
            break;
        default:
            std::cout << "\033[1;90m[LOG]\033[0m ";
            break;
    }

    ((std::cout << ' ' << std::forward<Args>(args)), ...);
    std::cout << std::endl;
#else
    (void)level;
    (void)std::initializer_list<int>{((void)args, 0)...};   
#endif // !NDEBUG    
}

template<typename... Args>
void fatal(Args... args){ log(LEVEL::FATAL, args...); }

template<typename... Args>
void error(Args... args){ log(LEVEL::ERROR, args...); }

template<typename... Args>
void warning(Args... args){ log(LEVEL::WARNING, args...); }

template<typename... Args>
void info(Args... args){ log(LEVEL::INFO, args...); }

template<typename... Args>
void verbose(Args... args){ log(LEVEL::VERBOSE, args...); }

template<typename... Args>
void debug(const char* file, u16 line, Args... args){
#ifndef NDEBUG
    LOG_PREFIX_DEBUG;
    std::cout << file << "::" << line; 
	((std::cout << ' ' << std::forward<Args>(args)), ...);
	std::cout << std::endl;
#else
    (void)file;
    (void)line;
    (void)std::initializer_list<int>{((void)args, 0)...};
#endif // !NDEBUG
}
// trick
#define debug(...) debug(__FILE__, __LINE__, __VA_ARGS__)

template<typename... Args>
void trace(Args... args){ log(LEVEL::TRACE, args...); }

}	// namespace log
}	// namespace hep
