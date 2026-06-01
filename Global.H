#ifndef GLOBAL_H
#define GLOBAL_H

#define SYSTEM_ERROR          "System error: "
#define UNABLE_TO             "Unable to "

#ifndef CONSOLE_OFF
#include <sstream>
#include "Time.H"
#define CONSOLE(msg)          \
{\
  struct timeval atv; struct tm atm; std::ostringstream oss;\
  Time::toLocalTime(Time::getTimeNow(&atv), &atm);\
  Time::toStringStream(&atv, &atm, oss)<<" "<<__FILE__<<":"<<__FUNCTION__<<"["<<__LINE__<<"] "; \
  int n=50-oss.tellp(); \
  while (n-->0) { oss.put(' '); } \
  oss<<msg; \
  std::cerr<<oss.str()<<std::endl;\
}
#define IF_CONSOLE(cond,msg)  { if (cond) { CONSOLE(msg); } }

#else // suppress console messages

#define CONSOLE(msg)
#define IF_CONSOLE(cond,msg)

#endif // CONSOLE_OFF

#endif // GLOBAL_H
