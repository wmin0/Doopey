#include "logger/Logger.h"

#include <cstdarg>
#include <cstdio>
#include <ctime>
#include <string>

using namespace Doopey;

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

  _logFile = fopen(tmp.data(), "w");
}

Logger::~Logger() {
  fclose(_logFile);
}

void Logger::error(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  writeLog(LL_Error, format, ap);
  va_end(ap);
}

void Logger::warning(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  writeLog(LL_Warning, format, ap);
  va_end(ap);
}

void Logger::info(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  writeLog(LL_Info, format, ap);
  va_end(ap);
}

void Logger::debug(const char* format, ...) {
  va_list ap;
  va_start(ap, format);
  writeLog(LL_Debug, format, ap);
  va_end(ap);
}

void Logger::writeLog(LogLevel level, const char* format, va_list ap) {
  if (level > _level) {
    return;
  }

  switch (level) {
    case LL_Error:
      fprintf(_logFile, "[Error] ");
      break;
    case LL_Warning:
      fprintf(_logFile, "[Warn] ");
      break;
    case LL_Info:
      fprintf(_logFile, "[Info] ");
      break;
    case LL_Debug:
      fprintf(_logFile, "[Debug] ");
      break;
    default:
      fprintf(_logFile, "[Unknown] ");
      break;
  }
  vfprintf(_logFile, format, ap);
}
