#include <iostream>
#include "Time.H"
#include "Console.H"



void Console::post(std::string msg)
{
  struct timeval atv;
  struct tm      atm;
  Time::getTimeNow(&atv);
  Time::toLocalTime(&atv, &atm);

  std::string buffer;
  Time::toString(&atv, &atm, buffer);

  std::cerr<<buffer<<__FILE__<<"/"<<__LINE__<<"/"<<__FUNCTION__<<"(): "<<msg<<std::endl;
}
