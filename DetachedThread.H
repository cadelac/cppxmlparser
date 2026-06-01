#ifndef DETACHEDTHREAD_H
#define DETACHEDTHREAD_H


/**
 * This class is a specific implementation of AttachmentPolicy (used by Runnable).
 */
class DetachedThread
{
protected:

  /**
   * This method makes the specified thread id detached, i.e. non-joinable
   */
  void detach(pthread_t threadId_) { pthread_detach(threadId_); }

  /**
   * Always returns true
   */
  bool isDetached() { return true; }

  /**
   * A detached thread's return value is not available
   */
  void* getReturnCode(int rc_)
  {
    return 0;
  }

  /**
   * A detached thread may not be joined
   */
  bool join(pthread_t threadId_, int& rc_) { rc_=0; return false; }
};

#endif // DETACHEDTHREAD_H
