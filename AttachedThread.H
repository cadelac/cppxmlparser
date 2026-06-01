#ifndef ATTACHEDTHREAD_H
#define ATTACHEDTHREAD_H


/**
 * This class is a specific implementation of AttachmentPolicy (used by Runnable).
 */
class AttachedThread
{
protected:

  /**
   * This method makes doesn't do anything.
   */
  void detach(pthread_t threadId_) {}

  /**
   * Always returns false
   */
  bool isDetached() { return false; }

  struct ReturnCode
  {
    ReturnCode(int rc_)
      : _rc(rc_) {}
    ~ReturnCode() {}
    int _rc;
  };

  /**
   * Capture the thread's return code
   */
  void* getReturnCode(int rc_)
  {
    return new ReturnCode(rc_);
  }

  /**
   * An attached thread may be joined
   */
  bool join(pthread_t threadId_, int& rc_)
  {
    ReturnCode* prc = 0;
    int code = pthread_join(threadId_, (void**)&prc);
    if (code==0) {
      if (prc != 0) {
	rc_ = prc->_rc;
	delete prc;
      }
      return true;
    }
    return false;
  }
};

#endif // ATTACHEDTHREAD_H
