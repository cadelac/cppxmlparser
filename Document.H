#ifndef DOCUMENT_H
#define DOCUMENT_H

#include <map>
#include <string>

class Document
{
public:
  Document();
  ~Document();

  struct Elements;
  Elements& getElements() { return _elements; }



  struct Attribute
  {
  };
  typedef std::map<std::string,Attribute*> Attributes;

  struct Content
  {
  };

  struct Element;

  struct Elements : public Content
  {
    Element& addElement(std::string const&);
    typedef std::map<std::string,Element*> ElementMap;
    ElementMap                _elementMap;
  };

  struct TextValue : public Content
  {
    std::string               _value;
  };

  struct Element
  {
  public:
    Element(std::string const& name_);
    ~Element();
  private:
    std::string               _name;
    Attributes*               _attributes;
    Content*                  _content;
  }; // Element



private:
  Elements                    _elements;
};

#endif // DOCUMENT_H
