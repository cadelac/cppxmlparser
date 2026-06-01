#include <iostream>
#include <exception>

#include "Global.H"
#include "Time.H"



template <
  class AttachmentPolicy,
  class CleanUpPolicy,
  int StackSize
>
Runnable<AttachmentPolicy,CleanUpPolicy,StackSize>::Runnable() 
  : _threadId(0)
  , _isRunning(false)
{
  CONSOLE("Constructed Runnable");
}



template <
  class AttachmentPolicy,
  class CleanUpPolicy,
  int StackSize
>
Runnable<AttachmentPolicy,CleanUpPolicy,StackSize>::~Runnable() 
{
  _isRunning = false; 
  CONSOLE("Destroyed Runnable");
}



template <
  class AttachmentPolicy,
  class CleanUpPolicy,
  int StackSize
>
void Runnable<AttachmentPolicy,CleanUpPolicy,StackSize>::start()
{
  pthread_attr_t attr;
  pthread_attr_init(&attr);
 
  size_t stackSize = (StackSize==0) ? DEFAULT_STACK_SIZE : StackSize;
  int rc = pthread_attr_setstacksize(&attr, stackSize);
  IF_CONSOLE((rc==0), "Thread stack size set to [" << stackSize << "]");
  IF_CONSOLE((rc!=0), SYSTEM_ERROR UNABLE_TO "set thread stack size [" << stackSize << "]");

  pthread_attr_t* pattr = 0;
  if (rc == 0)
    pattr = &attr;

  rc = pthread_create(&_threadId, pattr, Runnable::threadEntryPoint, this);
  IF_CONSOLE((rc==0), "Create thread: " << _threadId);
  IF_CONSOLE((rc!=0), SYSTEM_ERROR UNABLE_TO "create thread");

  if (rc == 0) {
    AttachmentPolicy::detach(_threadId);
    _isRunning = true;
    CONSOLE("Started Runnable");
  }
}



template <
  class AttachmentPolicy,
  class CleanUpPolicy,
  int StackSize
>
bool Runnable<AttachmentPolicy,CleanUpPolicy,StackSize>::join(int& rc_)
{	
  if (pthread_self() != _threadId &&
      AttachmentPolicy::join(_threadId, rc_))
    return true;

  return false;
}



template <
  class AttachmentPolicy,
  class CleanUpPolicy,
  int StackSize
>
void* Runnable<AttachmentPolicy,CleanUpPolicy,StackSize>::threadEntryPoint(void* runnable_)
{
   CONSOLE("Runnable::threadEntryPoint()");

   Runnable* runnable = static_cast<Runnable*>(runnable_);
   IF_CONSOLE( (runnable==0), SYSTEM_ERROR "runnable is null");

   void* prc = 0;

   if (runnable) {

     runnable->_isRunning = true;

     try {
       runnable->prologue();
       prc = runnable->AttachmentPolicy::getReturnCode(runnable->run());
       runnable->epilogue();
     }

     catch(std::exception const& except_) {
       runnable->catchException(&except_);
     }

     catch(...) {
       runnable->catchException(0);
     }

     runnable->_isRunning = false;

     if (runnable->CleanUpPolicy::isAutomaticallyReaped()) {
       CONSOLE("Reaping runnable");
       delete runnable;
     }
   }

   return prc;
};



template <
  class AttachmentPolicy,
  class CleanUpPolicy,
  int StackSize
>
void Runnable<AttachmentPolicy,CleanUpPolicy,StackSize>::catchException(std::exception const* except_)
{
  IF_CONSOLE((except_==0), SYSTEM_ERROR "thread [" << _threadId <<"]: unknown exception");
  IF_CONSOLE((except_!=0), SYSTEM_ERROR "thread [" << _threadId <<"]: exception [" << except_->what() << "]");
}
