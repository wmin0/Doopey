#ifndef _DOOPEY_LOGGER_H_
#define _DOOPEY_LOGGER_H_

#include <string>
#include <cstdarg>

using std::string;

namespace Doopey {

  enum LogLevel {
    LL_None = 0,
    LL_Error = 1,
    LL_Warning = 2,
    LL_Info = 3,
    LL_Debug = 4
  }; // enum LogLevel

  class Logger {

    public:
      Logger(LogLevel level, const char* dir = "");
      ~Logger();
      void error(const char* format, ...);
      void warning(const char* format, ...);
      void info(const char* format, ...);
      void debug(const char* format, ...);

    private:
      void writeLog(LogLevel level, const char* format, va_list ap);

    private:
      LogLevel _level;
      string _dir;
      FILE* _logFile;

  }; // class Logger

};  // namespace Doopey

#endif
