#include "logger/Logger.h"

#include <cstdio>
#include <ctime>
#include <cstdarg>
#include <string>

#include <iostream>

using namespace Doopey;
using std::string;

Logger::Logger(LogLevel level, const char* dir):
  _level(level), _dir(dir) {
  
  if ("" == _dir) {
    _logFile = stderr;
    return;
  }


  time_t cur;
  time(&cur);
  struct tm* timeinfo = localtime(&cur);
  char name[256];
  strftime(name, 256, "/Log_%y%m%d_%H%M%S.txt", timeinfo);

  string tmp = _dir;
  tmp += name;

  std::cerr << "here!" << std::endl;

  _logFile = fopen(tmp.data(), "w");
}

Logger::~Logger() {
  fclose(_logFile);
}

void Logger::error(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  writeLog(Error, format, ap);
  va_end(ap);
}

void Logger::warning(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  writeLog(Warning, format, ap);
  va_end(ap);
}

void Logger::info(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  writeLog(Info, format, ap);
  va_end(ap);
}

void Logger::debug(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  writeLog(Debug, format, ap);
  va_end(ap);
}

void Logger::writeLog(LogLevel level, const char* format, va_list ap) {
  if (level > _level) {
    return;
  }

  switch (level) {
    case Error:
      fprintf(_logFile, "[Error] ");
      break;
    case Warning:
      fprintf(_logFile, "[Warn] ");
      break;
    case Info:
      fprintf(_logFile, "[Info] ");
      break;
    case Debug:
      fprintf(_logFile, "[Debug] ");
      break;
    default:
      fprintf(_logFile, "[Unknown] ");
      break;
  }
  vfprintf(_logFile, format, ap);
}
