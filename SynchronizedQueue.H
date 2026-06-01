#ifndef SYNCHRONIZEDQUEUE_H
#define SYNCHRONIZEDQUEUE_H

#include "Waitable.H"
#include "CircularStorage.H"

template <class T, int N>
class SynchronizedQueue : public Waitable<CircularStorage<T,N>,ObjectLevelLockable>
{
public:
  SynchronizedQueue(){}
  ~SynchronizedQueue(){}

  void put(T const& item_)
  {
    if (CircularStorage<T,N>::length() >= N)
      Waitable<CircularStorage<T,N>,ObjectLevelLockable>::wait();

    CircularStorage<T,N>::push(item_);
    Waitable<CircularStorage<T,N>,ObjectLevelLockable>::notify();
  }

  T const& get()
  {
    if (CircularStorage<T,N>::length() < 1)
      Waitable<CircularStorage<T,N>,ObjectLevelLockable>::wait();

    T* item = CircularStorage<T,N>::pop();
    Waitable<CircularStorage<T,N>,ObjectLevelLockable>::notify();
    return *item;
  }
};

#endif // SYNCHRONIZEDQUEUE_H
