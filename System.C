#include <unistd.h>
#include "System.H"

std::string System::_hostName;
std::string System::_loginName;
std::string System::_cwd;
pid_t       System::_pid;


std::string const& System::getHostName()
{
  bool firstTime = true;
  if (firstTime) {
    char buf[256];
    if (gethostname(buf, sizeof(buf)) == 0)
      _hostName = buf;
    firstTime = false;
  }
  return _hostName; 
}

std::string const& System::getLoginName()
{
  bool firstTime = true;
  if (firstTime) {
    char buf[256];
    if (getlogin_r(buf, sizeof(buf)) == 0)
      _loginName = buf;
    firstTime = false;
  }
  return _loginName; 
}

std::string const& System::getWorkingDirectory()
{
  bool firstTime = true;
  if (firstTime) {
    char buf[8*1024];
    getcwd(buf, sizeof(buf));
    _cwd = buf;
    firstTime = false;
  }
  return _cwd;
}


pid_t System::getPid() 
{
  bool firstTime = true;
  if (firstTime) {
    _pid = getpid();
    firstTime = false;
  }
  return _pid; 
}
