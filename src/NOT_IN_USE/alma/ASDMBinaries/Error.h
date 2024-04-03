#if     !defined(_ERROR_H)

#include <string>
#include <stdarg.h>

enum ErrorLevel { NOERROR=0, WARNING, MINOR, SERIOUS, FATAL };

class Error
{
 public:

  Error();
  Error(ErrorLevel errlev);
  Error(ErrorLevel errlev, std::string message);
  Error(ErrorLevel errlev, char* fmt, ... );
  Error(int errlev);
  Error(int errlev, std::string message);
  Error(int errlev, char* fmt, ... );
  ~Error();

  void   notify(ErrorLevel errlev, std::string message);
  void   notify(std::string message);

  static void       setAcceptableLevel(ErrorLevel errlev);
  static ErrorLevel getAcceptableLevel();
  static std::string     getAcceptableLevelToString();

  static ErrorLevel getLevel();
  static std::string     getLevelToString();
  static std::string     getErrorMessage();

  static void       clearMessage();
  static void       clearErrLev();

 private:
  static ErrorLevel acceptableErrorLevel;
  static std::string     errorMessage_;
  static ErrorLevel errlev_;
  std::string            errorLog;
  void              printMessage(std::string message);
};

#define _ERROR_H
#endif
