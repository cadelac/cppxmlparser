#ifndef SYSTEM_H
#define SYSTEM_H

#include <string>

class System
{
public:
  static std::string const& getHostName();
  static std::string const& getLoginName();
  static std::string const& getWorkingDirectory();
  static pid_t getPid();

private:
  static std::string          _hostName;
  static std::string          _loginName;
  static std::string          _cwd;
  static pid_t                _pid;
};

#endif // SYSTEM_H
