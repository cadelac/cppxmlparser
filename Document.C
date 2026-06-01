#include "Document.H"

Document::Element::Element(std::string const& name_)
  : _name(name_)
  , _attributes(0)
  , _content(0)
{
}

Document::Element& Document::Elements::addElement(std::string const& elementName_)
{
  Element* elem = new Element(elementName_);
  _elementMap.insert( make_pair( elementName_, elem ));
  return *elem;
}


Document::Document()
{
}

Document::~Document()
{
}

