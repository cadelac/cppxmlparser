#include <iostream>
#include <iomanip>
#include <sstream>

#include "Global.H"
#include "Time.H"



struct timeval* Time::getTimeNow(struct timeval* ptv)
{
  if (ptv != 0) {
#ifndef CONSOLE_OFF
    int rc = 
#endif
    gettimeofday(ptv, 0);
    IF_CONSOLE( (rc!=0), SYSTEM_ERROR UNABLE_TO "get time of day" );
  }
  return ptv;
}



void Time::toLocalTime(struct timeval const* ptv, struct tm* ptm)
{
  if (ptv != 0 && ptm != 0) {
#ifndef CONSOLE_OFF
    struct tm* p = 
#endif
    localtime_r(&(ptv->tv_sec), ptm);
    IF_CONSOLE( (p==0), SYSTEM_ERROR UNABLE_TO "convert to local time" );
  }
}



std::ostringstream& Time::toStringStream(struct timeval* ptv, struct tm* ptm, std::ostringstream& oss)
{
  char save_fill = oss.fill();

  oss.setf(std::ios_base::right,std::ios_base::adjustfield);

  oss.fill('0');
  oss << std::setw(2) << ptm->tm_mon+1    << "/";
  oss << std::setw(2) << ptm->tm_mday     << "/";
  oss << std::setw(2) << ptm->tm_year%100 << " ";
  oss << std::setw(2) << ptm->tm_hour     << ":";
  oss << std::setw(2) << ptm->tm_min      << ":";
  oss << std::setw(2) << ptm->tm_sec      << ".";
  oss << std::setw(6) << ptv->tv_usec;
  oss << std::setw(0);

  oss.fill(save_fill);
  return oss;
}

std::string& Time::toString(struct timeval* ptv, struct tm* ptm, std::string& buffer)
{
  std::ostringstream oss;

  char save_fill = oss.fill();

  oss.setf(std::ios_base::right,std::ios_base::adjustfield);

  oss.fill('0');
  oss << std::setw(2) << ptm->tm_mon+1    << "/";
  oss << std::setw(2) << ptm->tm_mday     << "/";
  oss << std::setw(2) << ptm->tm_year%100 << " ";
  oss << std::setw(2) << ptm->tm_hour     << ":";
  oss << std::setw(2) << ptm->tm_min      << ":";
  oss << std::setw(2) << ptm->tm_sec      << ".";
  oss << std::setw(6) << ptv->tv_usec     << " ";
  oss << std::setw(0);

  oss.fill(save_fill);
  buffer = oss.str();
  return buffer;
}
