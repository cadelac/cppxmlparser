#ifndef WAITABLE_H
#define WAITABLE_H



/**
 * Objects derived from Waitable can be conditionally waited upon
 */
template <
  class T,
  template <class> class LockLevelPolicy
>
class Waitable : public T, public LockLevelPolicy<T>
{
public:
  Waitable();
  virtual ~Waitable();
		
  int unsafeWait();
  void wait();
		
  bool unsafeTimedWait(unsigned int ms);
  bool timedWait(unsigned int ms);

  void unsafeNotify();
  void notify();

  void unsafeBroadcastNotify();
  void broadcastNotify();


	
private:
  pthread_cond_t              _condition;
  bool                        _isAlive;
};

#include "Waitable.C"

#endif	
