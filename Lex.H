#ifndef LEX_H
#define LEX_H

#include <string>
#include <string_view>
#include <exception>


class Lex
{
public:
  typedef int Token;

  class ParseError : public std::exception
  {
  public:
    ParseError(char const* message_, int line_, int column_) {
      _message = message_;
      _message += " at line " + std::to_string(line_) + ", column " + std::to_string(column_);
    }
    ParseError(std::string const& message_, int line_, int column_) {
      _message = message_;
      _message += " at line " + std::to_string(line_) + ", column " + std::to_string(column_);
    }
    ~ParseError() noexcept {}
    virtual const char* what() const noexcept { return _message.c_str(); }
    private:
    std::string _message;
  };



  Lex(char const* file_);
  ~Lex(){}

  Token lex(std::string_view& lexeme_);

  enum {
    LEFT_ANGLE_BRACKET  = '<',
    RIGHT_ANGLE_BRACKET = '>',
    EQUAL_SIGN          = '=',
    RIGHT_CURLY_BRACE   = '}',
    LEFT_SLASH=500,         // "</"
    RIGHT_SLASH,            // "/>"
    DOLLAR_OPEN,            // "${"
    IDENTIFIER,
    STRING,
    SEQUENCE,
    INTEGRAL,
    REAL,
    EOSTREAM
  };

  Token get();
  void unget(Token);
  Token getString(std::string_view& lexeme_);
  Token getIdentifier(std::string_view& lexeme_);
  Token getNumeric(std::string_view& lexeme_);
  Token getSequence(std::string_view& lexeme_);
  int getLine() const { return _line; }
  int getColumn() const { return _column; }
  int setMode(int mode_) { int mode=_mode; _mode=mode_; return mode;}

private:
  enum { BUFFERSIZE=32*1024 };

  int   _fd;
  char  _buffer[BUFFERSIZE];
  char* _cp;
  int   _ungetCount;
  Token _ungetToken;
  int   _remaining;
  int   _line;
  int   _column;
  int   _mode;
  std::string _tokenBuf; // reusable token accumulation buffer; views into this are valid until the next lex() call
};

#define THROW(x) throw ParseError(x, _line, _column)

#endif // LEX_H
