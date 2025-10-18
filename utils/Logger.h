#pragma once
#include <string>
#include <mutex>
#include <ctime>
#include <cstdarg>
#include <cstdio>
#include <map>

#include "common/Singleton.h"

enum class LogLevel {
    TRACE, DEBUG, INFO, WARN, ERROR, FATAL
};

class Logger : public pattern::Singleton<Logger> {
    MAKE_SINGLETON(Logger)

public:
    void setLevel(LogLevel level);
    void setQuiet(bool quiet);
    void addFileOutput(FILE* fp, LogLevel level);
    void log(LogLevel level, const char* file, int line, bool printErrno, const char* fmt, ...);

private:
    const char* getLevelString(LogLevel level);
    const char* getColorCode(LogLevel level);
    void writeLog(FILE* fp, LogLevel level, const char* file, int line, bool printErrno, const char* fmt, va_list args);

    std::mutex mutex_;
    LogLevel level_ = LogLevel::TRACE;
    bool quiet_ = false;
    std::map<FILE*, LogLevel> outputs_;
};

#define LOG_TRACE(...) Logger::getInstance().log(LogLevel::TRACE, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_DEBUG(...) Logger::getInstance().log(LogLevel::DEBUG, __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_INFO(...)  Logger::getInstance().log(LogLevel::INFO,  __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_WARN(...)  Logger::getInstance().log(LogLevel::WARN,  __FILE__, __LINE__, false, __VA_ARGS__)
#define LOG_ERROR(...) Logger::getInstance().log(LogLevel::ERROR, __FILE__, __LINE__, false,  __VA_ARGS__)
#define LOG_ERROR_NE(...) Logger::getInstance().log(LogLevel::ERROR, __FILE__, __LINE__, true,  __VA_ARGS__)
#define LOG_FATAL(...) Logger::getInstance().log(LogLevel::FATAL, __FILE__, __LINE__, false,  __VA_ARGS__)
#define LOG_SET_LEVEL(level) Logger::getInstance().setLevel(level)
#define LOG_SET_QUIET(quiet) Logger::getInstance().setQuiet(quiet)
#define LOG_ADD_FILE_OUTPUT(fp, level) Logger::getInstance().addFileOutput(fp, level)