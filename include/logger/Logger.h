#ifndef _DOOPEY_LOGGER_H_
#define _DOOPEY_LOGGER_H_

#include <string>
#include <cstdarg>

namespace Doopey {

  using std::string;

  enum LogLevel {
    None = 0,
    Error = 1,
    Warning = 2,
    Info = 3,
    Debug = 4
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

      LogLevel _level;
      string _dir;
      FILE* _logFile;

  }; // class Logger

};  // namespace Doopey

#endif
