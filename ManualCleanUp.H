#ifndef MANUALCLEANUP_H
#define MANUALCLEANUP_H



/**
 * This class is a specific implementation of CleanUpPolicy (used by Runnable).
 */
class ManualCleanUp
{
protected:
  bool isAutomaticallyReaped() { return false; }
};

#endif // MANUALCLEANUP_H
