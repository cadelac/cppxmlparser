#ifndef LOCKABLE_H
#define LOCKABLE_H

#include <pthread.h>


/**
 * Synchronization primitive
 */
class Lockable
{
public:
  Lockable();
  ~Lockable();
		
  void lock() const;
  void unlock() const;
    
  pthread_mutex_t& getMutex() const { return _mutex; }

protected:
    mutable pthread_mutex_t _mutex;
};

#endif	
