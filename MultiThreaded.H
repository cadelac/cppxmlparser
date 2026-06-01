#ifndef MULTITHREADED_H
#define MULTITHREADED_H



template <
  class T,
  template <class> class LockLevelPolicy
>
class MultiThreaded : public LockLevelPolicy<T>
{
public:

};

#endif // MULTITHREADED_H
