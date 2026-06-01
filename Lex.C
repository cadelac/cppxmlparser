#include <fcntl.h>
#include <unistd.h>
#include "Lex.H"



Lex::Lex(char const* file_)
  : _cp(_buffer)
  , _ungetCount(0)
  , _ungetToken(0)
  , _remaining(0)
  , _line(1)
  , _column(1)
  , _mode(0)
{
  _fd = open(file_, O_RDONLY);
}



Lex::Token Lex::get()
{
  if (_ungetCount > 0) {
    --_ungetCount;
    return _ungetToken;
  }

  if (_remaining < 1) { // fill buffer
    if ((_remaining = read(_fd, _buffer, sizeof(_buffer))) <= 0)
      return EOSTREAM;
    _cp = _buffer;
  }

  if (*_cp == '\n') {
    ++_line;
    _column=1;
  }
  else 
    ++_column;

  --_remaining;
  return *_cp++;
}



void Lex::unget(int ungetToken_)
{
  _ungetToken = ungetToken_;
  ++_ungetCount;
}



Lex::Token Lex::getString(std::string_view& lexeme_)
{
  _tokenBuf.clear();
  Token token;
  while ((token = get()) != EOSTREAM && token != '\'')
    _tokenBuf.push_back(static_cast<char>(token));
  lexeme_ = _tokenBuf;
  return STRING;
}



Lex::Token Lex::getIdentifier(std::string_view& lexeme_)
{
  _tokenBuf.clear();
  Token token;
  while ((token = get()) != EOSTREAM && (isalnum(token) || token=='_'))
    _tokenBuf.push_back(static_cast<char>(token));
  unget(token);
  lexeme_ = _tokenBuf;
  return IDENTIFIER;
}



Lex::Token Lex::getNumeric(std::string_view& lexeme_)
{
  _tokenBuf.clear();
  Token token;

  // integral part
  while ((token = get()) != EOSTREAM && (isdigit(token)))
    _tokenBuf.push_back(static_cast<char>(token));

  if (token == EOSTREAM || token != '.') {
    unget(token);
    lexeme_ = _tokenBuf;
    return INTEGRAL;
  }

  // fractional part
  unget(token = get());  // lookahead

  if (isdigit(token)) {
    _tokenBuf.push_back('.');
    while ((token = get()) != EOSTREAM && (isdigit(token)))
      _tokenBuf.push_back(static_cast<char>(token));
    unget(token);
  }

  lexeme_ = _tokenBuf;
  return REAL;
}



Lex::Token Lex::getSequence(std::string_view& lexeme_)
{
  _tokenBuf.clear();
  Token token, lookbehind=0;
  while ((token=get()) != EOSTREAM &&
	 (token != '<' || lookbehind=='\\') &&
	 (token != '$' || lookbehind=='\\')) {
    lookbehind = token;
    _tokenBuf.push_back(static_cast<char>(token));
  }
  unget(token);
  lexeme_ = _tokenBuf;
  return SEQUENCE;
}




Lex::Token Lex::lex(std::string_view& lexeme_)
{
  lexeme_ = {};
  Token token;
  while ((token = get()) != EOSTREAM) {
    if (!isspace(token)) {
      switch (token) {
      case '<':
	token = get(); // lookahead
	if (token == '/') {
	  _mode=0; // exit special mode
	  return LEFT_SLASH;
	}
	unget(token);
	return LEFT_ANGLE_BRACKET;
	
      case '>':
	_mode=1;// enter special mode
	return RIGHT_ANGLE_BRACKET;
	
      case '/':
	token = get(); // lookahead
	if (token != '>')
	  THROW("Unknown token");	
	return RIGHT_SLASH;

      case '=':
	return EQUAL_SIGN;

      case '$':
	token = get();
	if (token != '{')
	  THROW("Unknown token");	
	return DOLLAR_OPEN;

      case '}':
	return RIGHT_CURLY_BRACE;

      case '\'': // start of a string
	// scan for end of string
	return getString(lexeme_);
	
      default:
	unget(token);
	if (_mode==0) {
	  if (isalpha(token)) // start of IDENTIFIER
	    return getIdentifier(lexeme_);
	
	  if (isdigit(token)) // start of NUMERIC
	    return getNumeric(lexeme_);
	}
	else {// _mode == 1 
	  // start of SEQUENCE
	  return getSequence(lexeme_);
	}
	
	break;
      }
    }
  }

  return EOSTREAM;
}
