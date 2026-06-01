#ifndef AUTOCLEANUP_H
#define AUTOCLEANUP_H



/**
 * This class is a specific implementation of CleanUpPolicy (used by Runnable).
 */
class AutoCleanUp
{
protected:
  bool isAutomaticallyReaped() { return true; }
};

#endif // AUTOCLEANUP_H
