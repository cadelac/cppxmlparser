#include <iostream>

#include "Global.H"
#include "Time.H"
#include "System.H"
#include "Log.H"

char* Log::severityTable[] = {
  "Emergency",
  "Alert",
  "Critical",
  "Error",
  "Warning",
  "Notice",
  "Info",
  "Debug",
  ""
};


#define LINE_OF_DASHES        "--------------------------------------------------------------------------------"

void Log::init()
{
  std::ostream* os;
  if (_sink._type == STANDARD )
    os = _sink._handle._os;

  else if (_sink._type == FILESYSTEM )
    os = _sink._handle._ofs;

  else if (_sink._type == SYSLOG ) {
    return;
  }

  struct timeval atv;
  struct tm atm;
  Time::toLocalTime(Time::getTimeNow(&atv), &atm);
  std::ostringstream oss;
  Time::toStringStream(&atv, &atm, oss);

  if (os != 0) {
    ThreadingModel::Guard guard;
    (*os) << LINE_OF_DASHES << '\n' <<
      '\t' << System::getLoginName() << '@' <<
                System::getHostName() << ':' << System::getWorkingDirectory() << '\n' <<
      '\t' << oss.str() << ": " << _sink._ident << " ["<<System::getPid() << "]\n" <<
      '\t' << ThreadingModel::getLockLevel() << '\n' <<
      LINE_OF_DASHES << std::endl;
  }
}

Log::Severity severity(unsigned int severity_)
{
  return severity_;
}

Log& preamble(Log& log_)
{
  struct timeval atv;
  struct tm atm;
  Time::toLocalTime(Time::getTimeNow(&atv), &atm);

  ThreadingModel::lock();

  log_._oss.str("");
  log_._oss.seekp(0);
  Time::toStringStream(&atv, &atm, log_._oss);

  return log_;
}

Log& postamble(Log& log_)
{
  int n=65-log_._oss.tellp();
  while (n-->0) { log_._oss.put(' '); }
  return log_;
}

Log::Flush flush(unsigned int severity_) 
{
  return severity_;
}

Log& operator<<(Log& log_, char const* msg_)
{
  log_._oss << msg_;
  return log_;
}

Log& operator<<(Log& log_, std::string const& msg_)
{
  log_._oss << msg_;
  return log_;
}


Log& operator<<(Log& log_, int value_)
{
  log_._oss << value_;
  return log_;
}

Log& operator<<(Log& log_, unsigned int value_)
{
  log_._oss << value_;
  return log_;
}

Log& operator<<(Log& log_, float value_)
{
  log_._oss << value_;
  return log_;
}

Log& operator<<(Log& log_, double value_)
{
  log_._oss << value_;
  return log_;
}
