#include "Global.H"
#include "Time.H"
#include "Runnable.H"
#include "ObjectLevelLockable.H"
#include "ClassLevelLockable.H"
#include "SingleThreaded.H"
#include "MultiThreaded.H"
#include "SingletonHolder.H"
#include "Log.H"
#include "SynchronizedQueue.H"
#include "Document.H"
#include "Parser.H"


class Producer : public Runnable<>
{
public:
  typedef SynchronizedQueue<int,6> Queue;
  Producer(int id_, Queue& queue_) : _id(id_), _queue(queue_) {}
  ~Producer() {}
  int _id;

protected:
  int run()
  {
    for (int i=0; i<50; ++i) {
      _queue.put(i);
      LOG(Log::NOTICE, "Installed [" << _id << " : " << i << "]" );
      IF_LOG((i==49), Log::WARNING, "Last Iteration [" << _id << " : " << i << "]" );
    }

    return _id;
  }

private:
  Queue& _queue;
};

class Consumer : public Runnable<>
{
public:
  typedef SynchronizedQueue<int,6> Queue;
  Consumer(int id_, Queue& queue_) : _id(id_), _queue(queue_) {}
  ~Consumer() {}
  int _id;

protected:
  int run()
  {
    for (int i=0; i<50; ++i) {
      int i = _queue.get();
      LOG(Log::NOTICE, "Extracted [" << _id << " : " << i << "]" );
      IF_LOG((i==49), Log::WARNING, "Last Iteration [" << _id << " : " << i << "]" );
    }

    return _id;
  }

private:
  Queue& _queue;
};


int main(int argc, char* argv[])
{
  LOG_INSTANCE(log);
  log.open(Log::Filesystem("IBON", "/dev/tty"));  //  log.open(Log::Syslog("IBON"));
  log.init();
  log.setSeverity(Log::NOTICE);

  Parser p("xml.txt");
  p.init(argc, argv);
  try {
    Document doc;
    p.parse(doc);
  }
  catch (Parser::ParseError const& except_) {
    LOG(Log::ERR, "Parse error: [" << except_.what() << "]");
  }

#ifdef XXX
  Lockable aLock;
  {
    ObjectLevelLockable<Counter>::ObjectGuard guard( aLock );
  }


  SynchronizedQueue<int,6> sq;

  Producer* r1 = new Producer(1, sq);
  Consumer* r2 = new Consumer(2, sq);

  r1->start();
  r2->start();

  int rc=0;

  if (r1->join(rc)) {
    CONSOLE("Reaped r" << r1->_id << ": rc=" << rc);
    delete r1;
  }

  if (r2->join(rc)) {
    CONSOLE("Reaped r" << r2->_id << ": rc=" << rc);
    delete r2;
  }
#endif
}
