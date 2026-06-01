#ifndef RUNNABLE_H
#define RUNNABLE_H

#include <pthread.h>
#include "AttachedThread.H"
#include "ManualCleanUp.H"

class std::exception;



template <
  class AttachmentPolicy=AttachedThread,
  class CleanUpPolicy=ManualCleanUp,
  int StackSize=0
>
class Runnable : public AttachmentPolicy, public CleanUpPolicy
{
public:
  enum { DEFAULT_STACK_SIZE=500*1024 };

  Runnable();
  virtual ~Runnable();

  pthread_t getThreadID() const { return _threadId; };
  bool isRunning() const { return _isRunning;}

  void start();
  bool join(int& rc_);



protected:
  virtual void prologue(){}
  virtual int run() = 0;
  virtual void epilogue(){}



private:
  static void* threadEntryPoint(void*);
  void catchException(std::exception const* except_);



  pthread_t                   _threadId;
  volatile bool               _isRunning;
};

#include "Runnable.C"

#endif	
