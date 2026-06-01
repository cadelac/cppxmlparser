#ifndef SINGLETHREADED_H
#define SINGLETHREADED_H



class SingleThreaded
{
public:
  class ClassGuard
  {
  public:
    ClassGuard(){}
    ~ClassGuard(){}
  };

  static void lock() {}
  static void unlock() {}
  static char const* getLockLevel() { return "SingleThreaded"; }
};

#endif // SINGLETHREADED_H
