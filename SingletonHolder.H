#ifndef SINGLETONHOLDER_H
#define SINGLETONHOLDER_H



template <
  class T,
  class ThreadingPolicy
>
class SingletonHolder 
{
public:
  static T& instance()
  {
    if (_instance == 0) {
      typename ThreadingPolicy::Guard guard;
      if (_instance == 0)
	_instance = new T();
    }
    return *_instance;
  };



private:
  static T* _instance;
};



template <
  class T,
  class ThreadingPolicy
>
T* SingletonHolder<T,ThreadingPolicy>::_instance;

#endif // SINGLETONHOLDER_H
