#include <iostream>
#include "Global.H"
#include "Time.H"
#include "Lockable.H"



Lockable::Lockable()
{
  pthread_mutexattr_t mutex_attr;
  memset(&mutex_attr,0,sizeof(mutex_attr));

  int result = pthread_mutexattr_init(&mutex_attr);
  IF_CONSOLE((result!=0), SYSTEM_ERROR "xxxUnable to initialize mutex attribute: rc=[" << result << "]");

  // initialize mutex to be recursive
  if (result == 0) {
    result = pthread_mutexattr_settype(&mutex_attr, PTHREAD_MUTEX_RECURSIVE);
    IF_CONSOLE((result!=0), SYSTEM_ERROR "Unable to set mutex attribute PTHREAD_MUTEX_RECURSIVE: rc=[" << result << "]");

    if (result == 0) {
      // initialize mutex with attribute
      result = pthread_mutex_init(&_mutex, &mutex_attr);

      IF_CONSOLE((result!=0), SYSTEM_ERROR "Unable to initialize mutex with attribute: rc=[" << result << "]");
      IF_CONSOLE((result==0), "Created mutex for lockable");
    }
  }
}



Lockable::~Lockable()
{
  // destroy lock
#ifndef CONSOLE_OFF
  int result = 
#endif
  pthread_mutex_destroy(&_mutex);
  IF_CONSOLE((result==0), "Destroyed mutex for lockable");
  IF_CONSOLE((result!=0), SYSTEM_ERROR "Unable to destroy mutex: rc=[" << result << "]");

}



void Lockable::lock() const
{
#ifndef CONSOLE_OFF
  int result = 
#endif
  pthread_mutex_lock(&_mutex);
  IF_CONSOLE((result==0), "Locked mutex for lockable");
  IF_CONSOLE((result!=0), SYSTEM_ERROR "Unable to lock mutex: rc=[" << result << "]");
}



void Lockable::unlock() const
{
#ifndef CONSOLE_OFF
  int result = 
#endif
  pthread_mutex_unlock(&_mutex);
  IF_CONSOLE((result==0), "Unlocked mutex for lockable");
  IF_CONSOLE((result!=0), SYSTEM_ERROR "Unable to unlock mutex: rc=[" << result << "]");
}
