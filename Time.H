#ifndef TIME_H
#define TIME_H

#include <sys/time.h>
#include <time.h>
#include <string>



class Time
{
public:
  static struct timeval* getTimeNow(struct timeval*);
  static void toLocalTime(struct timeval const*, struct tm*);
  static std::ostringstream& toStringStream(struct timeval*, struct tm*, std::ostringstream&);
  static std::string& toString(struct timeval*, struct tm*, std::string&);

};

#endif // TIME_H
