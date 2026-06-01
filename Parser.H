#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <string_view>
#include <map>

#include "Lex.H"
#include "Document.H"



class Parser
{
public:
  typedef Lex::Token Token;
  typedef Lex::ParseError ParseError;

  Parser(char const* file_);
  void init(int argc_, char* argv_[]);
  void parse(Document&);


private:

  enum {
    DOCUMENT=600,
    ELEMENT,
    ATTRIBUTE,
    CONTENT,
    VERBATIM,
    SEQUENCE,
    VALUE,
    NUMERIC,
    COMPOUND,
    SIMPLE
  };

  class Value {
  public:
    Value() {}
    virtual ~Value() {}

    virtual std::string_view asString() const = 0;
  };


  // T is one of { int double std::string }
  template <class T>
  class TypedValue : public Value {
  public:
    TypedValue() : Value(), _value() {}
    TypedValue(T const& value_) : Value(), _value(value_) {}

    std::string_view asString() const;

    // must return a reference
    T const& getValue() const { return _value; }
  protected:
    T                              _value;
    mutable std::string            _str; // cached string form for numeric types
  };


  typedef TypedValue<int>          IntValue;
  typedef TypedValue<double>       RealValue;
  typedef TypedValue<std::string>  StringValue;

  typedef std::map<std::string,std::string> EnvironmentMap;


  class Ref {
  public:
    Ref(std::string const& identifier_)
      : _identifier(identifier_) {}
    std::string const& getIdentifier() const { return _identifier; }
    StringValue resolve() const { 
      EnvironmentMap::const_iterator iter = _envMap.find(_identifier);
      if (iter != _envMap.end()) 
	return StringValue(iter->second);
      char *var = ::getenv(_identifier.c_str());
      if (var == 0)
	return StringValue();
      return StringValue(var);
    }
  private:
    // refers to this identifier
    std::string const              _identifier;
  };

  //class Ref; // forward declaration
  //typedef TypedValue<Ref>          RefValue;




  class KeyValuePair {
  public:
    KeyValuePair(std::string const& key_, Value const& value_)
      : _key(key_), _value(value_) {}
    virtual ~KeyValuePair() {}
    std::string const& getKey() const { return _key; }
    virtual Value const& getValue() const { return _value; }
  protected:
    std::string                    _key;
    Value const&                   _value;
  };

  // T is an instance of IntValue, RealValue, StringValue
  template <class T>
  class Attr : public KeyValuePair {
  public:
    Attr(std::string const& key_, T const& value_)
      : KeyValuePair(key_, value_), _value(value_) {}
    T const& getValue() const { return Attr::_value; }
  protected:
    T _value;
  };

  typedef Attr<IntValue>           IntAttr;
  typedef Attr<RealValue>          RealAttr;
  typedef Attr<StringValue>        StringAttr;
  //typedef Attr<RefValue>           RefAttr;


  Token getDocument(Document&);
  Token getElements(Document::Elements&);
  Token getElement(Document::Elements&, std::string& elementName_);
  void getAttributes();
  Ref getReference();
  void getContent(Document::Elements&);


  Token getNext(std::string_view& lexeme_);
  Token lookahead(std::string_view& lexeme_);
  Token pushBack(Token, std::string_view lexeme_);

  Lex                              _lex;
  bool                             _lookaheadIsAvailable;
  std::string                      _lookaheadLexeme; // owned storage for pushed-back token
  Token                            _lookaheadToken;

  static EnvironmentMap            _envMap;
};

#undef THROW
#define THROW(x) throw ParseError(x, _lex.getLine(), _lex.getColumn())

#endif // PARSER_H
