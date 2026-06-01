#ifndef LOG_H
#define LOG_H

#include <syslog.h>
#include <iostream>
#include <sstream>
#include <fstream>

#include "Global.H"
#include "Time.H"

#ifdef MULTITHREADED
#include "ClassLevelLockable.H"
#include "MultiThreaded.H"
class Log;
typedef MultiThreaded<Log,ClassLevelLockable> ThreadingModel;
#else // SINGLETHREADED
#include "SingleThreaded.H"
typedef SingleThreaded ThreadingModel;
#endif

#include "SingletonHolder.H"







class Log
{
private:
  static char* severityTable[9];

public:
  void init();

  enum SINK {
    STANDARD,   // standard error <cerr>
    FILESYSTEM, // flat file
    SYSLOG,     // syslog facility
    SOCKET      // process via tcp/ip
  };

  // open a file
  struct Filesystem {
    Filesystem(char const* ident_, char const* file_):_ident(ident_), _file(file_){}
    char const* _ident;
    char const* _file;
  };
  void open(Filesystem const& fs_) { _sink.open(fs_); }

  struct Syslog {
    Syslog(char const* ident_):_ident(ident_){}
    char const* _ident;
  };

  // send to syslog
  void open(Syslog const& sl_){ _sink.open(sl_); }

  enum SEVERITY {
    EMERG=0,
    ALERT,
    CRIT,
    ERR,
    WARNING,
    NOTICE,
    INFO,
    DEBUG
  };

  enum SEVERITY_TABLE { SEVERITY_TABLE_SIZE=sizeof(severityTable)/sizeof(char*)-1 };

  unsigned int getSeverity() const { return _severity; }

  char const* severityAsString(unsigned int severity_) 
  { int index = (severity_<SEVERITY_TABLE_SIZE) ?
      severity_ : SEVERITY_TABLE_SIZE; return severityTable[index];}

  void setSeverity(unsigned int severity_)  { _severity=severity_; }

  struct Severity {
    Severity(unsigned int severity_) : _severity(severity_) {}
    unsigned int _severity;
  };
  Log& operator<<(Severity const& sev_) 
  {
    _severity=sev_._severity;
    CONSOLE("Severity set to [" << _severity << "]");
    return *this;
  } 

  friend Severity severity(unsigned int severity_);

  typedef Log& (*OPER)(Log&);
  Log& operator<<(OPER oper) { return (*oper)(*this); } 

  /**
   * put preamble information: date, time, file, function, line
   */
  friend Log& preamble(Log&);
  friend Log& postamble(Log&);
  //  friend Log& flush(Log&);

  struct Flush {
    Flush(unsigned int severity_):_severity(severity_){}
    unsigned int _severity;
  };
  Log& operator<<(Flush const& flush_)
  {
    _sink.write(_oss, flush_._severity);
    ThreadingModel::unlock();
    return *this;
  }


  friend Log& operator<<(Log&,char const*);
  friend Log& operator<<(Log&,std::string const&);
  friend Log& operator<<(Log&,int);
  friend Log& operator<<(Log&,unsigned int);
  friend Log& operator<<(Log&,float);
  friend Log& operator<<(Log&,double);



private:
  /**
   * allow SingletonHolder to create a Log object
   */
  friend class SingletonHolder<Log, ThreadingModel>;

  /**
   * constructor is private so that only
   * SingletonHolder can create a Log object
   */
  Log() : _oss(), _severity(WARNING), _sink()
  { CONSOLE("Created Log: Severity=" << severityAsString(_severity)); }

  // declared but not defined
  Log(Log const&);
  Log& operator=(Log const&);


  struct Sink {
    Sink() : _isOpen(false),_ident(0),_type(STANDARD) { _handle._os = &std::cerr; }

    void open(Filesystem const& fs_)
    {
      _ident = fs_._ident;
      if (fs_._file != 0 && *fs_._file != '\0' && (_handle._ofs = new std::ofstream(fs_._file)) != 0) {
	if (!_handle._ofs->fail()) {
	  _isOpen = true;
	  _type = FILESYSTEM;
	}
      } 
    }

    // send to syslog
    void open(Syslog const& sl_)
    {
      _ident = sl_._ident;
      openlog(_ident, LOG_PID, LOG_USER );
      _isOpen = true;
      _type = SYSLOG;
    }

    void close()
    {
      if (_isOpen) {
	switch (_type) {
	case STANDARD:
	  break;
	case FILESYSTEM:
	  _handle._ofs->close();
	  delete _handle._ofs;
	  break;
	case SYSLOG:
	  closelog();
	  break;
	case SOCKET:
	  break;
	}

	_isOpen = false;
	_handle._os = &std::cerr;
      }
    }

    void write(std::ostringstream& oss_, unsigned int severity_) 
    {
      if (_type == STANDARD) {
	if (_handle._os != 0) {
	  (*_handle._os) << oss_.str() << std::endl;
	}
      }
      else if (_type == FILESYSTEM ) {
	if (_handle._ofs != 0) {
	  (*_handle._ofs) << oss_.str() << std::endl;
	}
      }
      else if (_type == SYSLOG ) {
	syslog(severity_, oss_.str().c_str()+25);
      }
    }

    bool _isOpen;
    char const* _ident;
    SINK _type; // type of sink
    union {
      std::ostream*  _os;  // used by STANDARD
      std::ofstream* _ofs; // used by FILESYSTEM
    } _handle;
  };

  std::ostringstream          _oss;
  unsigned int                _severity;
  Sink                        _sink;
};



Log::Severity severity(unsigned int severity_);

Log& preamble(Log&);
Log& postamble(Log&);
Log::Flush flush(unsigned int);

Log& operator<<(Log&,char const*);
Log& operator<<(Log&,std::string const&);
Log& operator<<(Log&,int);
Log& operator<<(Log&,unsigned int);
Log& operator<<(Log&,float);
Log& operator<<(Log&,double);


#define LOG_INSTANCE(log)     Log& log(SingletonHolder<Log,ThreadingModel>::instance());


#define LOG(sev,msg)          {\
  LOG_INSTANCE(log);\
  if (sev <= log.getSeverity()) {					\
    log<<preamble<<" "<<__FILE__<<":"<<__FUNCTION__<<"["<<__LINE__<<"] <"; \
    if ((unsigned int)sev < (unsigned int)Log::SEVERITY_TABLE_SIZE) \
      log<<log.severityAsString(sev);\
    else \
      log<<sev;\
    log<<"> "<<postamble<<msg<<flush(sev);\
  }\
}
#define IF_LOG(cond,severity,msg)      { if (cond) { LOG(severity,msg); } }

#endif // LOG_H

