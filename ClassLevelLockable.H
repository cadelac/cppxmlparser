#ifndef CLASSLEVELLOCKABLE_H
#define CLASSLEVELLOCKABLE_H

#include "Lockable.H"



template <
  class T
>
class ClassLevelLockable
{
public:
  class Guard
  {
  public:
    Guard()
      : _lock(ClassLevelLockable<T>::_lock)
    { _lock.lock(); }
    ~Guard(){ _lock.unlock(); }

  private:
    Lockable const& _lock;
  };

  static char const* getLockLevel() { return "MultiThreaded[Class Level Lockable]"; }
  static void lock() { _lock.lock(); }
  static void unlock() { _lock.unlock(); }


protected:
  ClassLevelLockable(){}
  ~ClassLevelLockable(){}

  static Lockable _lock;
};


template<class T>
Lockable ClassLevelLockable<T>::_lock;

#endif // CLASSLEVELLOCKABLE_H
