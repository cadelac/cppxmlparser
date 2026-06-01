#include <errno.h>
#include <iostream>
#include <sys/time.h>

#include "Global.H"
#include "Time.H"
#include "Waitable.H"



template <
  class T,
  template <class> class LockLevelPolicy
>
Waitable<T,LockLevelPolicy>::Waitable()
  : T(), LockLevelPolicy<T>(), _isAlive(false)
{
  int result = 0;

  // initialize condition attribute
  pthread_condattr_t condattrDetails;
  memset(&condattrDetails, 0, sizeof(condattrDetails));

  result = pthread_condattr_init(&condattrDetails);
  IF_CONSOLE((result!=0), SYSTEM_ERROR UNABLE_TO "initialize condition attribute: rc=[" << result << "]");

  if (result==0) {
    result = pthread_cond_init(&_condition, &condattrDetails);
    IF_CONSOLE((result!=0), SYSTEM_ERROR UNABLE_TO "initialize condition variable: rc=[" << result << "]");

    if (result==0) {
      result = pthread_condattr_destroy(&condattrDetails);
      IF_CONSOLE((result!=0), SYSTEM_ERROR UNABLE_TO "destroy condition attribute: rc=[" << result << "]");

      if (result==0)
	_isAlive = true;
    }
  }
}



template <
  class T,
  template <class> class LockLevelPolicy
>
Waitable<T,LockLevelPolicy>::~Waitable()
{
  _isAlive = false;

  // destroy condition
  int result = 0;
  while ((result = pthread_cond_destroy(&_condition)) == EBUSY)    // a thread is
    broadcastNotify();                      // waiting on this condition variable

  IF_CONSOLE((result != 0), SYSTEM_ERROR UNABLE_TO "destroy condition variable: rc=[" << result << "]");
}


	
template <
  class T,
  template <class> class LockLevelPolicy
>
int Waitable<T,LockLevelPolicy>::unsafeWait()
{
  int result = pthread_cond_wait(&_condition, &LockLevelPolicy<T>::_lock.getMutex());
  IF_CONSOLE((result != 0), SYSTEM_ERROR UNABLE_TO "wait on condition: rc=[" << result << "]");
  return result;     
}



template <
  class T,
  template <class> class LockLevelPolicy
>
void Waitable<T,LockLevelPolicy>::wait()
{
  if (_isAlive) {
    LockLevelPolicy<T>::lock();

    if(_isAlive)
      unsafeWait();

    LockLevelPolicy<T>::unlock();
  }
}



template <
  class T,
  template <class> class LockLevelPolicy
>
bool Waitable<T,LockLevelPolicy>::unsafeTimedWait(unsigned int ms)
{
  if (_isAlive) {
    struct timespec timeout;
    struct timeval  tp;
    gettimeofday(&tp, NULL);

    timeout.tv_sec   = (ms / 1000) + tp.tv_sec;
    timeout.tv_nsec  = ((ms % 1000) * 1000000) + (tp.tv_usec * 1000);
    while (timeout.tv_nsec >= 1000000000) {
      timeout.tv_nsec -= 1000000000;
      timeout.tv_sec++;
    }

    int result = pthread_cond_timedwait(&_condition, &LockLevelPolicy<T>::_lock.getMutex(), &timeout);
    return (result != ETIMEDOUT);
  }

  return false;
}



template <
  class T,
  template <class> class LockLevelPolicy
>
bool Waitable<T,LockLevelPolicy>::timedWait(unsigned int ms)
{
  if (_isAlive) {
    LockLevelPolicy<T>::lock();
    bool result = unsafeTimedWait(ms);
    LockLevelPolicy<T>::unlock();
    return result;
  }

  return false;
}



template <
  class T,
  template <class> class LockLevelPolicy
>
void Waitable<T,LockLevelPolicy>::unsafeNotify()
{
#ifndef CONSOLE_OFF
  int result = 
#endif
  pthread_cond_signal(&_condition);
  IF_CONSOLE((result!=0), SYSTEM_ERROR UNABLE_TO "issue a signal to condition variable: rc=[" << result << "]");
}



template <
  class T,
  template <class> class LockLevelPolicy
>
void Waitable<T,LockLevelPolicy>::notify()
{
  LockLevelPolicy<T>::lock();
  unsafeNotify();
  LockLevelPolicy<T>::unlock();
}



template <
  class T,
  template <class> class LockLevelPolicy
>
void Waitable<T,LockLevelPolicy>::unsafeBroadcastNotify()
{
#ifndef CONSOLE_OFF
  int result = 
#endif
  pthread_cond_broadcast(&_condition);

  IF_CONSOLE((result != 0), SYSTEM_ERROR UNABLE_TO "broadcast a signal to condition variable: rc=[" << result << "]");
}        



template <
  class T,
  template <class> class LockLevelPolicy
>
void Waitable<T,LockLevelPolicy>::broadcastNotify()
{
  LockLevelPolicy<T>::lock();
  unsafeBroadcastNotify();
  LockLevelPolicy<T>::unlock();
}
