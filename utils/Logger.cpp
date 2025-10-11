#include "Logger.h"
#include <cstring>
#include <cerrno>

Logger::~Logger(){
    // Cleanup if necessary
}

void Logger::setLevel(LogLevel level) {
    level_ = level;
}
    std::mutex mutex_;
    LogLevel level_;
    bool quiet_;
    std::map<FILE*, LogLevel> outputs_;
void Logger::setQuiet(bool quiet) {
    quiet_ = quiet;
}

void Logger::addFileOutput(FILE* fp, LogLevel level) {
    outputs_[fp] = level;
}

void Logger::log(LogLevel level, const char* file, int line, bool printErrno, const char* fmt, ...) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!quiet_ && level >= level_) {
        va_list args;
        va_start(args, fmt);
        writeLog(stderr, level, file, line, printErrno, fmt, args);
        va_end(args);
    }

    for (const auto& [fp, minLevel] : outputs_) {
        if (level >= minLevel) {
            va_list args;
            va_start(args, fmt);
            writeLog(fp, level, file, line, printErrno, fmt, args);
            va_end(args);
        }
    }
}

void Logger::writeLog(FILE* fp, LogLevel level, const char* file, int line, bool printErrno, const char* fmt, va_list args) {
    char timeBuf[64];
    time_t t = time(nullptr);
    struct tm* lt = localtime(&t);
    strftime(timeBuf, sizeof(timeBuf), "%Y-%m-%d %H:%M:%S", lt);

#ifdef LOG_USE_COLOR
    fprintf(fp, "%s %s%-5s\x1b[0m \x1b[90m%s:%d:\x1b[0m ", timeBuf, getColorCode(level), getLevelString(level), file, line);
#else
    fprintf(fp, "%s %-5s %s:%d: ", timeBuf, getLevelString(level), file, line);
#endif

    if (printErrno && level >= LogLevel::ERROR) {
        fprintf(fp, " [errno %d: %s] ", errno, strerror(errno));
    }

    vfprintf(fp, fmt, args);
    fprintf(fp, "\n");
    fflush(fp);
}

const char* Logger::getLevelString(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE: return "TRACE";
        case LogLevel::DEBUG: return "DEBUG";
        case LogLevel::INFO:  return "INFO";
        case LogLevel::WARN:  return "WARN";
        case LogLevel::ERROR: return "ERROR";
        case LogLevel::FATAL: return "FATAL";
        default: return "UNKNOWN";
    }
}

const char* Logger::getColorCode(LogLevel level) {
    switch (level) {
        case LogLevel::TRACE: return "\x1b[94m";
        case LogLevel::DEBUG: return "\x1b[36m";
        case LogLevel::INFO:  return "\x1b[32m";
        case LogLevel::WARN:  return "\x1b[33m";
        case LogLevel::ERROR: return "\x1b[31m";
        case LogLevel::FATAL: return "\x1b[35m";
        default: return "\x1b[0m";
    }
}
