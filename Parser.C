#include "Log.H"
#include "Parser.H"

Parser::EnvironmentMap Parser::_envMap;



Parser::Parser(char const* file_)
  : _lex(file_), _lookaheadIsAvailable(false), _lookaheadLexeme(), _lookaheadToken(0)
{
}



void Parser::init(int argc_, char* argv_[])
{
  bool inside = false;
  for (int i=0; i<argc_; ++i) {
    if (!inside) {
      if (strcmp(argv_[i], "{") == 0)
	inside = true;
    }
    else { // inside
      if (strcmp(argv_[i], "}") == 0)
	inside = false;
      else {
	std::string temp(argv_[i]);
	size_t p = temp.find('=');
	if (p != std::string::npos) { // found
	  _envMap.insert(std::make_pair(temp.substr(0,p), temp.substr(p+1)));
	  LOG(Log::NOTICE, "Environment map inserted [" << temp.substr(0,p) << "," << temp.substr(p+1) << "]");
	}
      }
    }
  }
}



void Parser::parse(Document& doc_)
{
  getDocument(doc_);
}



Parser::Token Parser::getElements(Document::Elements& elements_)
{
  Token token;
  std::string elementName;
  std::string_view elementNameView;
  while ((token = lookahead(elementNameView)) == Lex::LEFT_ANGLE_BRACKET) {
    if ((token = getElement(elements_, elementName)) == ELEMENT) {
      LOG(Log::NOTICE, "Element [" << elementName << "]");
    }
  }

  return token;
}


Parser::Token Parser::getDocument(Document& doc_)
{
  Token token = getElements(doc_.getElements());
  if (token  != Lex::EOSTREAM)
    THROW("End of input expected");

  return token;
}



Parser::Token Parser::getElement(Document::Elements& elements_, std::string& elementName_)
{
  std::string_view lexeme;

  Token token = getNext(lexeme);
  if (token == Lex::EOSTREAM)
    return Lex::EOSTREAM;

  if (token != Lex::LEFT_ANGLE_BRACKET)
    THROW("\'<\' expected");

  token = getNext(lexeme);
  if (token != Lex::IDENTIFIER)
    THROW("Identifier expected");
  elementName_ = lexeme;
  Document::Element& anElement(elements_.addElement(elementName_));

  token = lookahead(lexeme);
  if (token == Lex::IDENTIFIER)
    getAttributes();

  token = lookahead(lexeme); // refresh the lookahead
  if (token == Lex::RIGHT_SLASH) { // shortformat
    token = getNext(lexeme); // consume the lookahead
    return ELEMENT;
  }

  // must be longformat
  if (token != Lex::RIGHT_ANGLE_BRACKET)
    THROW("\'>\' expected");
  token = getNext(lexeme); // consume the lookahead

  // now we have seen the 'head'
  // see if there is 'content'
  token = lookahead(lexeme);

  if (token != Lex::LEFT_SLASH)
    getContent(elements_);

  token = lookahead(lexeme);
  if (token != Lex::LEFT_SLASH)
    THROW("\"</\" expected");
  token = getNext(lexeme); // consume the lookahead


  token = lookahead(lexeme);
  if (token != Lex::IDENTIFIER)
    THROW("Identifier expected" );
  if (elementName_ != lexeme)
    THROW("Identifier ["+ std::string(lexeme) +"] does not match previous identifier ["+ elementName_ + "]");
  token = getNext(lexeme); // consume the lookahead


  token = getNext(lexeme);
  if (token != Lex::RIGHT_ANGLE_BRACKET)
    THROW("\'>\' expected");

  return ELEMENT;
}



void Parser::getAttributes()
{
  Token token;
  std::string_view lexeme;
  std::string_view keyView;

  while (true) {
    token = lookahead(lexeme);
    if (token != Lex::IDENTIFIER)
      break;
    token = getNext(keyView); // consume the lookahead
    std::string key(keyView);

    token = getNext(lexeme);
    if (token != Lex::EQUAL_SIGN)
      THROW("\'=\' expected");

    token = getNext(lexeme);

    if (token == Lex::INTEGRAL) {
      IntAttr attr(key, std::stoi(std::string(lexeme)));
      LOG(Log::NOTICE, "Int Attr key [" << attr.getKey() << "], value ["<< attr.getValue().getValue() <<"]");
    }

    else if (token == Lex::REAL) {
      RealAttr attr(key, std::stod(std::string(lexeme)));
      LOG(Log::NOTICE, "Real Attr key [" << attr.getKey() << "], value ["<< attr.getValue().getValue() <<"]");
    }

    else if (token == Lex::STRING || token == Lex::DOLLAR_OPEN) {

      pushBack(token, lexeme);

      std::string runningString;

      while ((token = lookahead(lexeme)) == Lex::STRING || token == Lex::DOLLAR_OPEN) {
	if (token == Lex::STRING) {
	  runningString.append(lexeme);
	  token = getNext(lexeme); // consume the lookahead
	}
	else { // Lex::DOLLAR_OPEN
          Ref ref(getReference());
 	  runningString.append(ref.resolve().getValue());
	}
      }
      StringAttr attr(key, runningString);
      LOG(Log::NOTICE, "String Attr key [" << attr.getKey() << "], value ["<< attr.getValue().getValue() <<"]");
    }

    else
      THROW("\""+ std::string(lexeme) +"\" unexpected");
  }
}



Parser::Ref Parser::getReference()
{
  Token token;
  std::string_view lexeme;
  std::string_view valueView;

  if ((token=getNext(lexeme)) != Lex::DOLLAR_OPEN)
    THROW("\'${\' expected");
  if ((token=getNext(valueView)) != Lex::IDENTIFIER)
    THROW("Identifier expected");
  if ((token=getNext(lexeme)) != Lex::RIGHT_CURLY_BRACE)
    THROW("\'}\' expected");

  return Ref(std::string(valueView));
}



void Parser::getContent(Document::Elements& elements_)
{
  Token token;
  std::string_view lexeme;

  token = lookahead(lexeme);
  if (token == Lex::LEFT_ANGLE_BRACKET)
    getElements(elements_);
  else {// verbatim
    std::string runningString;
    while ((token = lookahead(lexeme)) == Lex::SEQUENCE || token == Lex::DOLLAR_OPEN) {
      if (token == Lex::SEQUENCE) {
	runningString.append(lexeme);
	token = getNext(lexeme); // consume the lookahead
      }
      else { // Lex::DOLLAR_OPEN
	int mode = _lex.setMode(0); // go back to regular mode '0'
	Ref ref(getReference());
	_lex.setMode(mode); // restore to previous mode
	runningString.append(ref.resolve().getValue());
      }
    }
    StringValue verbatim(runningString);
    LOG(Log::NOTICE, "Verbatim sequence [" << verbatim.getValue() << "]");
  }
}



Parser::Token Parser::getNext(std::string_view& lexeme_)
{
  if (_lookaheadIsAvailable) {
    _lookaheadIsAvailable = false;
    lexeme_ = _lookaheadLexeme;
    return _lookaheadToken;
  }

  return _lex.lex(lexeme_);
}

Parser::Token Parser::lookahead(std::string_view& lexeme_)
{
  if (_lookaheadIsAvailable) {
    lexeme_ = _lookaheadLexeme;
    return _lookaheadToken;
  }

  Token token = _lex.lex(lexeme_);
  return pushBack(token, lexeme_);
}

Parser::Token Parser::pushBack(Token token_, std::string_view lexeme_)
{
  if (_lookaheadIsAvailable)
    THROW("pushBack() overflow");

  _lookaheadLexeme = lexeme_;
  _lookaheadToken = token_;
  _lookaheadIsAvailable = true;
  return token_;
}


// specializations

template<>
std::string_view Parser::IntValue::asString() const
{
  _str = std::to_string(_value);
  return _str;
}

template<>
std::string_view Parser::RealValue::asString() const
{
  _str = std::to_string(_value);
  return _str;
}

template<>
std::string_view Parser::TypedValue<std::string>::asString() const
{
  return _value;
}

// template<>
// std::string const Parser::RefValue::asString() const
// {
//   return _value.getIdentifier();
// }
