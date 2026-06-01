#ifndef OBJECTLEVELLOCKABLE_H
#define OBJECTLEVELLOCKABLE_H

//#include "ClassLevelLockable.H"
#include "Lockable.H"


template <
  class T
>
class ObjectLevelLockable //: public ClassLevelLockable<T>
{
public:
  class Guard
  {
  public:
    Guard()
      : _lock(ObjectLevelLockable<T>::_lock)
    { _lock.lock(); }
    ~Guard(){ _lock.unlock(); }

    Guard(Lockable& lock_)
      : _lock(lock_) 
    { _lock.lock(); }
    ~Guard(){ _lock.unlock(); }

  private:
    Lockable const& _lock;
  };

  static char const* getLockLevel() { return "MultiThreaded[Object Level Lockable]"; }
  void lock() { _lock.lock(); }
  void unlock() { _lock.unlock(); }



protected:
  ObjectLevelLockable(){}
  ~ObjectLevelLockable(){}

  Lockable _lock;
};

#endif // OBJECTLEVELLOCKABLE_H
