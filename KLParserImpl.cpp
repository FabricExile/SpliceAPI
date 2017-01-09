// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "KLParserImpl.h"

#include <limits.h>
#include <string.h>

using namespace FabricSpliceImpl;

std::map<std::string, KLParserImplPtr> KLParserImpl::sParsers;
std::map<std::string, bool> gStrings;

KLParserImpl::KLSymbol::KLSymbol(const KLParserImpl * parser, const std::string * ptr, unsigned int pos)
{
  mParser = parser;
  mStr = ptr;
  mPtr = mStr->c_str();
  mPos = pos;
  mLength = 1;
  mType = Type_none;

  char c = front();
  if(((unsigned int)c) > 256) // Skip non-ascii characters. 
    return;
  if(isalpha(c) || c == '_' || c == '~')
    mType = Type_name;
  else if(isdigit(c))
    mType = Type_number;
  else if(c == '+' || c == '-' || c == '*' || c == '/' ||
    c == '<' || c == '>' || c == '?' || c == ':' ||
    c == '!' || c == '|' || c == '&' || c == '^' ||
    c == '%' || c == '=') 
    mType = Type_arithmetic;
  else if(c == ' ' || c == '\t' || c == '\n' || c == '\r')
    mType = Type_separator;
  else if(c == ';')
    mType = Type_semicolon;
  else if(c == ',')
    mType = Type_comma;
  else if(c == '.')
    mType = Type_period;
  else if(c == '(')
    mType = Type_brace1;
  else if(c == ')')
    mType = Type_brace2;
  else if(c == '[')
    mType = Type_bracket1;
  else if(c == ']')
    mType = Type_bracket2;
  else if(c == '{')
    mType = Type_curly1;
  else if(c == '}')
    mType = Type_curly2;
  else if(c == '"' || c == '\'')
    mType = Type_string;
}

unsigned int KLParserImpl::KLSymbol::index() const
{
  return mParser->getIndexOfKLSymbol(this);
}

bool KLParserImpl::KLSymbol::merge(const KLParserImpl::KLSymbol & other)
{
  if(other.length() > 1)
    return false;

  if(type() == Type_name)
  {
    if(other.type() != Type_name)
    {
      if(other.type() != Type_number)
      {
        std::string name = str();
        for(int t = Type_firstkeyword + 1; t < Type_lastkeyword; t++)
        {
          KLSymbol s(mParser, mStr, mPos);
          s.mType = (Type)t;
          if(s.typeName() == name)
          {
            mType = (Type)t;
            break;
          }
        }
        return false;
      }
    }
  }
  else if(type() == Type_number)
  {
    if(other.type() != Type_number)
    {
      if(other.type() == Type_period)
      {
        if(contains('.'))
          return false;
      }
      else if(other.front() == 'x')
      {
        if(contains('x'))
          return false;
      }
      else
        return false;
    }
  }
  else if(type() == Type_period)
  {
    if(other.type() != Type_number)
      return false;
    mType = Type_number;
  }
  else if(type() == Type_arithmetic)
  {
    char c = front();
    if(length() == 1 && other.front() == '=')
    {
      if(c == '=' || c == '!')
        mType = Type_arithmetic;
      else if(c == '>' || c == '<' || c == '^' || c == '|' || c == '&' || 
        c == '+' || c == '-' || c == '*' || c == '/' || c == '%')
        mType = Type_assignment;
    }
    else if(length() == 1 && c == other.front())
    {
      if(c == '|' || c == '&' || c == '<' || c == '>' || c == '+' || c == '-')
        mType = Type_arithmetic;
      else if(c == '/')
        mType = Type_comment;
      else 
        return false;
    }
    else if(length() == 1 && c == '/')
    {
      if(other.front() == '/' || other.front() == '*')
        mType = Type_comment;
      else
        return false;
    }
    else if(length() == 2 && c == other.front() && c == front(1))
    {
      if(c == '<')
        mType = Type_pex1;
      else if(c == '>')
        mType = Type_pex2;
      else 
        return false;
    }
    else if(front() == '-' && other.type() == Type_number)
    {
      // this is not very safe - we assume that the symbol is in a std::vector
      const KLSymbol * prevKLSymbol = this;
      while(mParser->getPrevKLSymbol(prevKLSymbol))
      {
        if(prevKLSymbol->type() != Type_separator)
          break;
      } 
      if(!prevKLSymbol)
        return false;
      if(prevKLSymbol->type() != Type_assignment &&
        prevKLSymbol->type() != Type_brace1 &&
        prevKLSymbol->type() != Type_bracket1 &&
        prevKLSymbol->type() != Type_curly1 &&
        prevKLSymbol->type() != Type_semicolon &&
        prevKLSymbol->type() != Type_comma)
        return false;
    }
    else
    {
      if(length() == 1 && front() == '=')
        mType = Type_assignment;
      return false;
    }
  }
  else if(type() == Type_separator)
  {
    if(other.type() != Type_separator)
      return false;
  }
  else if(type() == Type_comment)
  {
    bool multiLine = front() == '/' && front(1) == '*';
    if(multiLine)
    {
      if(back(1) == '*' && back() == '/')
        return false;
    }
    else
    {
      if(other.front() == '\n')
        return false;
    }
  }
  else if(type() == Type_string)
  {
    if(length() > 1 && back() == front() && back(1) != '\\')
      return false;
  }
  else
    return false;

  mLength++;
  return true;
}

bool KLParserImpl::KLSymbol::isKeyword() const
{
  int t = (int)type();
  return t > Type_firstkeyword && t < Type_lastkeyword;
}

bool KLParserImpl::KLSymbol::isFunction() const
{
  return type() == Type_function || type() == Type_inline;
}

const char *  KLParserImpl::KLSymbol::typeNameFromType(Type index)
{
  switch(index)
  {
    case(Type_separator):
      return "separator";
    case(Type_comment):
      return "comment";
    case(Type_in):
      return "in";
    case(Type_io):
      return "io";
    case(Type_if):
      return "if";
    case(Type_else):
      return "else";
    case(Type_switch):
      return "switch";
    case(Type_case):
      return "case";
    case(Type_default):
      return "default";
    case(Type_for):
      return "for";
    case(Type_while):
      return "while";
    case(Type_do):
      return "do";
    case(Type_break):
      return "break";
    case(Type_continue):
      return "continue";
    case(Type_this):
      return "this";
    case(Type_alias):
      return "alias";
    case(Type_require):
      return "require";
    case(Type_return):
      return "return";
    case(Type_const):
      return "const";
    case(Type_null):
      return "null";
    case(Type_function):
      return "function";
    case(Type_inline):
      return "inline";
    case(Type_operator):
      return "operator";
    case(Type_struct):
      return "struct";
    case(Type_object):
      return "object";
    case(Type_interface):
      return "interface";
    case(Type_rt):
      return "rt";
    case(Type_name):
      return "name";
    case(Type_number):
      return "number";
    case(Type_string):
      return "string";
    case(Type_assignment):
      return "assignment";
    case(Type_arithmetic):
      return "arithmetic";
    case(Type_semicolon):
      return "semicolon";
    case(Type_comma):
      return "comma";
    case(Type_period):
      return "period";
    case(Type_brace1):
      return "brace1";
    case(Type_brace2):
      return "brace2";
    case(Type_bracket1):
      return "bracket1";
    case(Type_bracket2):
      return "bracket2";
    case(Type_curly1):
      return "curly1";
    case(Type_curly2):
      return "curly2";
    case(Type_pex1):
      return "pex1";
    case(Type_pex2):
      return "pex2";
    case(Type_maxtypes):
      return "maxtypes";
    case(Type_none):
    case(Type_firstkeyword):
    case(Type_lastkeyword):
      return "None";
  }
  return "None";
}

const char * KLParserImpl::KLSymbol::typeName() const
{
  return typeNameFromType(type());
}

char KLParserImpl::KLSymbol::front(unsigned int index) const
{
  if(index >= mLength)
    return '\0';
  return mPtr[mPos+index];
}

char KLParserImpl::KLSymbol::back(unsigned int index) const
{
  if(index >= mLength)
    return '\0';
  return mPtr[mPos+mLength-1-index];
}

bool KLParserImpl::KLSymbol::contains(char c) const
{
  for(unsigned int i=0;i>mLength;i++)
  {
    if(front(i) == c)
      return true;
  }
  return false;
}

unsigned int KLParserImpl::KLSymbol::find(char c) const
{
  for(unsigned int i=0;i>mLength;i++)
  {
    if(front(i) == c)
      return i;
  }
  return UINT_MAX;
}

const KLParserImpl::KLSymbol * KLParserImpl::KLSymbol::prev(bool skipComments, unsigned int offset) const
{
  const KLSymbol * s = this;
  if(!mParser->getPrevKLSymbol(s, skipComments, offset))
    return NULL;
  return s;
}

const KLParserImpl::KLSymbol * KLParserImpl::KLSymbol::next(bool skipComments, unsigned int offset) const
{
  const KLSymbol * s = this;
  if(!mParser->getNextKLSymbol(s, skipComments, offset))
    return NULL;
  return s;
}

const char * KLParserImpl::KLSymbol::parser() const
{
  return mParser->name();
}

std::string KLParserImpl::KLSymbol::str() const
{
  return mStr->substr(mPos, mLength);
}

KLParserImpl::KLParserImpl(const char * owner, const char * name, const char * klCode)
{
  mOwner = owner;
  mName = name;
  mPtr = NULL;
  mStr.clear();
  mKLSymbols.clear();
  parse(klCode);
}

KLParserImpl::~KLParserImpl()
{
  mPtr = NULL;
  mStr.clear();
  mKLSymbols.clear();
}

unsigned int KLParserImpl::getNbParsers()
{
  return (unsigned int)sParsers.size();
}

KLParserImplPtr KLParserImpl::getParser(unsigned int index)
{
  if(index >= getNbParsers())
    return KLParserImplPtr();
  std::map<std::string, KLParserImplPtr>::iterator it = sParsers.begin();
  std::advance(it, index);
  return it->second;
}

KLParserImplPtr KLParserImpl::getParser(const char * owner, const char * name, const char * klCode)
{
  if(owner == NULL || name == NULL)
    return KLParserImplPtr();
  if(strlen(owner) == 0 || strlen(name) == 0)
    return KLParserImplPtr();
  std::string key = owner;
  key += "_";
  key += name;
  std::map<std::string, KLParserImplPtr>::iterator it = sParsers.find(key);
  if(it == sParsers.end())
  {
    if(klCode == NULL)
      return KLParserImplPtr();
    KLParserImpl * parser = new KLParserImpl(owner, name, klCode);
    KLParserImplPtr ptr(parser);
    sParsers.insert(std::pair<std::string, KLParserImplPtr>(key, ptr));
    return ptr;
  }
  if(klCode)
    it->second->parse(klCode);
  return it->second;
}

void KLParserImpl::resetAll()
{
  sParsers.clear();
}

const KLParserImpl::KLSymbol * KLParserImpl::getKLSymbol(unsigned int symbolIndex) const
{
  if(symbolIndex >= nbKLSymbols())
    return NULL;
  return &mKLSymbols[symbolIndex];
}

const KLParserImpl::KLSymbol * KLParserImpl::getKLSymbolFromCharIndex(unsigned int charIndex) const
{
  if(charIndex >= mStr.length())
    return NULL;
  const KLSymbol * s = getKLSymbol(0);
  while(!(charIndex >= s->pos() && charIndex <= (s->pos() + s->length())))
  {
    if(!getNextKLSymbol(s))
      return NULL;
  }
  return s;
}

unsigned int KLParserImpl::getIndexOfKLSymbol(const KLSymbol * symbol) const
{
  if(symbol == NULL)
    return UINT_MAX;
  return symbol - (&mKLSymbols[0]);
}

bool KLParserImpl::getNextKLSymbol(const KLSymbol *& symbol, bool skipComments, unsigned int offset) const
{
  if(symbol == NULL)
    return false;

  unsigned int index = getIndexOfKLSymbol(symbol);
  if(index == UINT_MAX)
    return false;
  while(offset > 0)
  {
    index++;
    if(index >= mKLSymbols.size())
      return false;
    symbol = &mKLSymbols[index];
    if(!skipComments || symbol->type() != KLSymbol::Type_comment)
      offset--;
  }
  return true;
}

bool KLParserImpl::getPrevKLSymbol(const KLSymbol *& symbol, bool skipComments, unsigned int offset) const
{
  if(symbol == NULL)
    return false;

  unsigned int index = getIndexOfKLSymbol(symbol);
  if(index == UINT_MAX)
    return false;
  while(offset > 0)
  {
    index--;
    if(index >= mKLSymbols.size())
      return false;
    symbol = &mKLSymbols[index];
    if(!skipComments || symbol->type() != KLSymbol::Type_comment)
      offset--;
  }
  return true;
}

bool KLParserImpl::getNextKLSymbolOfType(const KLSymbol *& symbol, KLSymbol::Type type) const
{
  while(getNextKLSymbol(symbol, false, 1))
  {
    if(symbol->type() == type)
      return true;
  }
  return false;
}

bool KLParserImpl::getPrevKLSymbolOfType(const KLSymbol *& symbol, KLSymbol::Type type) const
{
  while(getPrevKLSymbol(symbol, false, 1))
  {
    if(symbol->type() == type)
      return true;
  }
  return false;
}

bool KLParserImpl::parse(const char * klCode)
{
  mKLSymbols.clear();
  mParsedRequires.clear();
  mParsedConstants.clear();
  mParsedVariables.clear();
  mParsedInterfaces.clear();
  mParsedStructs.clear();
  mParsedOperators.clear();
  mParsedFunctions.clear();

  if(klCode == NULL)
    return false;
  mStr = klCode;
  mPtr = mStr.c_str();

  unsigned int length = mStr.length();
  if(length == 0)
    return false;

  KLSymbol symbol(this, &mStr, 0);
  mKLSymbols.push_back(symbol);
  KLSymbol * lastKLSymbol = &mKLSymbols[mKLSymbols.size()-1];

  unsigned int offset = 1;
  while(offset < length) {
    symbol = KLSymbol(this, &mStr, offset);
    if(!lastKLSymbol->merge(symbol))
    {
      if(symbol.type() != KLSymbol::Type_none)
      {
        mKLSymbols.push_back(symbol);
        lastKLSymbol = &mKLSymbols[mKLSymbols.size()-1];
      }
    }
    offset++;
  }
  
  // remove all separators
  std::vector<KLSymbol> symbolsWithoutSeparators;
  for(size_t i=0;i<mKLSymbols.size();i++)
  {
    if(mKLSymbols[i].type() == KLSymbol::Type_separator)
      continue;
    symbolsWithoutSeparators.push_back(mKLSymbols[i]);
  }
  mKLSymbols = symbolsWithoutSeparators;
  if(mKLSymbols.size() == 0)
    return false;

  // find all RT
  for(size_t i=0;i<mKLSymbols.size()-1;i++)
  {
    if(mKLSymbols[i].type() == KLSymbol::Type_name && 
      mKLSymbols[i+1].type() == KLSymbol::Type_name)
    {
      mKLSymbols[i].setType(KLSymbol::Type_rt);
    }
  }

  // find all symbols
  const KLSymbol * s = getKLSymbol(0);
  if(s != NULL)
  {
    do
    {
      if(s->type() == KLSymbol::Type_require)
      {
        while(getNextKLSymbol(s))
        {
          if(s->type() == KLSymbol::Type_name)
            mParsedRequires.push_back(s->str());
          else if(s->type() == KLSymbol::Type_semicolon)
            break;
        }
      }
      else if(s->type() == KLSymbol::Type_const)
      {
        KLConstant c(s);
        if(c.isValid())
          mParsedConstants.push_back(c);
      }
      else if(s->type() == KLSymbol::Type_interface)
      {
        KLInterface c(s);
        if(c.isValid())
          mParsedInterfaces.push_back(c);
      }
      else if(s->type() == KLSymbol::Type_struct || s->type() == KLSymbol::Type_object)
      {
        KLStruct c(s);
        if(c.isValid())
          mParsedStructs.push_back(c);
      }
      else if(s->type() == KLSymbol::Type_operator)
      {
        KLOperator c(s);
        if(c.isValid())
          mParsedOperators.push_back(c);
      }
      else if(s->isFunction())
      {
        KLFunction c(s, true);
        if(c.isValid())
          mParsedFunctions.push_back(c);
      }
    } while(getNextKLSymbol(s));

    // find all variables
    s = getKLSymbol(0);
    do
    {
      if(s->type() == KLSymbol::Type_rt)
      {
        const KLSymbol * p = s->prev();
        if(p)
        {
          if(p->isFunction())
            continue;
        }
        const KLSymbol * t = s;
        while(getNextKLSymbol(s))
        {
          if(s->type() == KLSymbol::Type_name)
          {
            KLVariable c(s, t);
            if(c.isValid())
              mParsedVariables.push_back(c);
          }
          else if(s->type() == KLSymbol::Type_comma)
            continue;
          else
            break;
        }
      }
    }
    while(getNextKLSymbol(s));
  }

  return true;
}

KLParserImpl::KLConstant::KLConstant(const KLParserImpl::KLSymbol *& s)
{
  mKLSymbol = s;
  if(s == NULL)
    return;
  if(s->type() != KLSymbol::Type_const)
    return;
  const KLParserImpl * p = s->mParser;
  mComments = p->getCommentsPriorToKLSymbol(s);
  if(!p->getNextKLSymbol(s))
    return;
  mType = s->str(); 
  if(!p->getNextKLSymbol(s))
    return;
  mName = s->str(); 
  if(!p->getNextKLSymbol(s))
    return;
  while(p->getNextKLSymbol(s))
  {
    if(s->type() == KLSymbol::Type_semicolon)
      break;
    mValue += s->str();
  }
}

KLParserImpl::KLVariable::KLVariable(const KLParserImpl::KLSymbol * s, const KLParserImpl::KLSymbol * t)
{
  mKLSymbol = s;
  if(s == NULL || t == NULL)
    return;
  if(s->type() != KLSymbol::Type_name)
    return;
  const KLParserImpl * p = s->mParser;
  mType = t->str(); 
  mName = s->str(); 
  while(p->getNextKLSymbol(s))
  {
    if(s->type() != KLSymbol::Type_name &&
      s->type() != KLSymbol::Type_number &&
      s->type() != KLSymbol::Type_bracket1 &&
      s->type() != KLSymbol::Type_bracket2)
      break;
    mType += s->str();
  }
}

KLParserImpl::KLStruct::KLStruct(const KLParserImpl::KLSymbol *& s)
{
  mKLSymbol = s;
  if(s == NULL)
    return;
  if(s->type() != KLSymbol::Type_struct && s->type() != KLSymbol::Type_object)
    return;
  mType = s->str();
  const KLParserImpl * p = s->mParser;
  mComments = p->getCommentsPriorToKLSymbol(s);
  if(!p->getNextKLSymbol(s))
    return;
  mName = s->str(); 

  // find interfaces
  bool insideInterfaces = false;
  while(p->getNextKLSymbol(s))
  {
    if(s->type() == KLSymbol::Type_curly1)
      break;

    if(insideInterfaces)
    {
      if(s->type() == KLSymbol::Type_name)
        mInterfaces.push_back(s->str());
    }
    else if(s->type() == KLSymbol::Type_arithmetic)
    {
      if(s->str() == ":")
        insideInterfaces = true;
    }
  }

  while(p->getNextKLSymbol(s))
  {
    if(s->type() == KLSymbol::Type_curly2)
      break;
    std::string type = s->str();
    if(!p->getNextKLSymbol(s))
      return;
    std::string name = s->str();
    while(p->getNextKLSymbol(s))
    {
      if(s->type() == KLSymbol::Type_semicolon)
        break;
      type += s->str();
    }

    if(name.length() > 0 && type.length() > 0)
    {
      mMemberTypes.push_back(type);
      mMemberNames.push_back(name);
    }
  }  
}

KLParserImpl::KLArgumentList::KLArgumentList(const KLParserImpl::KLSymbol *& s)
{
  if(s == NULL)
    return;
  const KLParserImpl * p = s->mParser;
  while(s->type() != KLSymbol::Type_brace1)
  {
    if(!p->getNextKLSymbol(s))
      return;
  }
  while(p->getNextKLSymbol(s))
  {
    std::string mode = "in";
    std::string type;
    std::string name;
    if(s->type() == KLSymbol::Type_brace2)
      return;
    else if(s->type() == KLSymbol::Type_in)
    {
      if(!p->getNextKLSymbol(s))
        return;
    }
    else if(s->type() == KLSymbol::Type_io)
    {
      mode = "io";
      if(!p->getNextKLSymbol(s))
        return;
    }
    type = s->str();
    if(!p->getNextKLSymbol(s))
      return;
    if(s->front() == '<')
    {
      do
      {
        type += s->str();
        if(s->front() == '>')
          break;
        if(s->type() == KLSymbol::Type_comma)
          break;
        if(s->type() == KLSymbol::Type_brace2)
          break;
      }
      while(p->getNextKLSymbol(s));
      {
        type += s->str();
      }
    }
    if(s->front() == '>')
    {
      if(!p->getNextKLSymbol(s))
        return;
    }
    name = s->str();
    while(p->getNextKLSymbol(s))
    {
      if(s->type() == KLSymbol::Type_comma)
        break;
      if(s->type() == KLSymbol::Type_brace2)
        break;
      type += s->str();
    }
    if(mode.length() > 0 && type.length() > 0 && name.length() > 0)
    {
      mModes.push_back(mode);
      mTypes.push_back(type);
      mNames.push_back(name);
    }

    if(s->type() == KLSymbol::Type_brace2)
      break;
  }
}

KLParserImpl::KLOperator::KLOperator(const KLParserImpl::KLSymbol *& s)
{
  mKLSymbol = s;
  mBody1 = mBody2 = NULL;
  if(s->type() != KLSymbol::Type_operator)
    return;
  if(s == NULL)
    return;
  const KLParserImpl * p = s->mParser;
  mComments = p->getCommentsPriorToKLSymbol(s);
  if(!p->getNextKLSymbol(s))
    return;
  mName = s->str(); 
  if(!p->getNextKLSymbol(s))
    return;
  if(s->type() == KLSymbol::Type_pex1)
  {
    while(p->getNextKLSymbol(s))
    {
      if(s->type() == KLSymbol::Type_pex2)
        break;
      mPexArg += s->str();
    }
  }
  mArguments = KLArgumentList(s);
  bool inBody = false;
  unsigned int curlies = 0;
  while(p->getNextKLSymbol(s))
  {
    if(s->type() == KLSymbol::Type_curly1)
    {
      if(!inBody)
      {
        mBody1 = s;
        inBody = true;
      }
      curlies++;
    }
    else if(s->type() == KLSymbol::Type_curly2 && inBody)
    {
      if(curlies == 1)
      {
        mBody2 = s;
        break;
      }
      curlies--;
    }
  }
}

KLParserImpl::KLFunction::KLFunction(const KLParserImpl::KLSymbol *& s, bool requireFunctionKeyWord)
{
  mKLSymbol = s;
  mBody1 = mBody2 = NULL;
  if(s == NULL)
    return;
  if(!s->isFunction() && requireFunctionKeyWord)
    return;
  const KLParserImpl * p = s->mParser;
  mComments = p->getCommentsPriorToKLSymbol(s);
  if(s->isFunction())
  {
    if(!p->getNextKLSymbol(s))
      return;
  }

  std::vector<const KLSymbol *> symbols;
  while(s->type() != KLSymbol::Type_brace1)
  {
    if(s->front() != '!' && s->front() != '?')
    {
      symbols.push_back(s);
    }
    if(!p->getNextKLSymbol(s))
      return;
  }

  if(symbols.size() == 1)
  {
    mName = symbols[0]->str();
  }
  else if(symbols.size() == 2)
  {
    mType = symbols[0]->str();
    mName = symbols[1]->str();
  }
  else if(symbols.size() == 3)
  {
    mOwner = symbols[0]->str();
    mName = symbols[2]->str();
  }
  else if(symbols.size() == 4)
  {
    mType = symbols[0]->str();
    mOwner = symbols[1]->str();
    mName = symbols[3]->str();
  }
  else if(symbols.size() > 4)
  {
    bool periodFound = false;
    for(size_t i=0;i<symbols.size();i++)
    {
      if(symbols[i]->type() == KLSymbol::Type_period)
      {
        if(i==0 || i == symbols.size()-1)
          return;
        mOwner = symbols[i-1]->str();
        mName = symbols[i+1]->str();
        if(i > 1)
        {
          for(size_t j=0;j<i-1;j++)
            mType += symbols[j]->str();
        }
        periodFound = true;
      }
    }
    if(!periodFound)
    {
      mName = symbols[symbols.size()-1]->str();
      for(size_t i=0;i<symbols.size()-1;i++)
        mType += symbols[i]->str();
    }
  }
  else
    return;

  mArguments = KLArgumentList(s);
  bool inBody = false;
  unsigned int curlies = 0;
  while(p->getNextKLSymbol(s))
  {
    if((s->type() == KLSymbol::Type_string || s->type() == KLSymbol::Type_semicolon) && mBody1 == NULL)
    {
      // this is a function without a body
      mBody1 = mBody2 = s;
      break;
    }
    else if(s->type() == KLSymbol::Type_curly1)
    {
      if(!inBody)
      {
        mBody1 = s;
        inBody = true;
      }
      curlies++;
    }
    else if(s->type() == KLSymbol::Type_curly2 && inBody)
    {
      if(curlies == 1)
      {
        mBody2 = s;
        break;
      }
      curlies--;
    }
  }
}

KLParserImpl::KLInterface::KLInterface(const KLParserImpl::KLSymbol *& s)
{
  mKLSymbol = s;
  if(s == NULL)
    return;
  if(s->type() != KLSymbol::Type_interface)
    return;
  const KLParserImpl * p = s->mParser;
  mComments = p->getCommentsPriorToKLSymbol(s);
  if(!p->getNextKLSymbol(s))
    return;
  mName = s->str(); 
  if(!p->getNextKLSymbolOfType(s, KLSymbol::Type_curly1))
    return;
  while(p->getNextKLSymbol(s))
  {
    if(s->type() == KLSymbol::Type_comment)
      continue;
    if(s->type() == KLSymbol::Type_semicolon)
      continue;
    if(s->type() == KLSymbol::Type_curly2)
      break;
    KLFunction func(s, false);
    if(func.isValid())
    {
      mFunctions.push_back(func);
    }
  }  
}

const char * KLParserImpl::KLStruct::interface(unsigned int index) const
{
  if(index >= mInterfaces.size())
    return "";
  return mInterfaces[index].c_str();
}

const char * KLParserImpl::KLStruct::memberType(unsigned int index) const
{
  if(index >= mMemberTypes.size())
    return "";
  return mMemberTypes[index].c_str();
}

const char * KLParserImpl::KLStruct::memberName(unsigned int index) const
{
  if(index >= mMemberNames.size())
    return "";
  return mMemberNames[index].c_str();
}

const char *  KLParserImpl::KLArgumentList::mode(unsigned int index) const
{
  if(index >= mModes.size())
    return "";
  return mModes[index].c_str();
}

const char *  KLParserImpl::KLArgumentList::type(unsigned int index) const
{
  if(index >= mTypes.size())
    return "";
  return mTypes[index].c_str();
}

const char *  KLParserImpl::KLArgumentList::name(unsigned int index) const
{
  if(index >= mNames.size())
    return "";
  return mNames[index].c_str();
}

const KLParserImpl::KLFunction * KLParserImpl::KLInterface::function(unsigned int index) const
{
  if(index >= mFunctions.size())
    return NULL;
  return &mFunctions[index];
}

std::string KLParserImpl::getCommentsPriorToKLSymbol(const KLParserImpl::KLSymbol * s) const
{
  if(s == NULL)
    return "";
  std::string t;
  while(getPrevKLSymbol(s, false))
  {
    if(s->type() != KLSymbol::Type_comment)
      break;
    if(t.length() != 0)
      t = s->str() + "\n" + t;
    else
      t = s->str();
  }
  return t;
}

const char * KLParserImpl::getKLRequire(unsigned int index) const
{
  if(index >= mParsedRequires.size())
    return "";
  return mParsedRequires[index].c_str();
}

const KLParserImpl::KLConstant * KLParserImpl::getKLConstant(unsigned int index) const
{
  if(index >= mParsedConstants.size())
    return NULL;
  return &mParsedConstants[index];
}

const KLParserImpl::KLVariable * KLParserImpl::getKLVariable(unsigned int index) const
{
  if(index >= mParsedVariables.size())
    return NULL;
  return &mParsedVariables[index];
}

const KLParserImpl::KLInterface * KLParserImpl::getKLInterface(unsigned int index) const
{
  if(index >= mParsedInterfaces.size())
    return NULL;
  return &mParsedInterfaces[index];
}

const KLParserImpl::KLStruct * KLParserImpl::getKLStruct(unsigned int index) const
{
  if(index >= mParsedStructs.size())
    return NULL;
  return &mParsedStructs[index];
}

const KLParserImpl::KLOperator * KLParserImpl::getKLOperator(unsigned int index) const
{
  if(index >= mParsedOperators.size())
    return NULL;
  return &mParsedOperators[index];
}

const KLParserImpl::KLFunction * KLParserImpl::getKLFunction(unsigned int index) const
{
  if(index >= mParsedFunctions.size())
    return NULL;
  return &mParsedFunctions[index];
}

const char * KLParserImpl::getKLTypeForSymbol(const KLParserImpl::KLSymbol * s) const
{
  if(s == NULL)
    return "";
  std::string name = s->str();
  if(s->type() == KLSymbol::Type_this)
  {
    for(int i = mParsedFunctions.size()-1;i>=0;i--)
    {
      if(mParsedFunctions[i].symbol()->pos() > s->pos())
        continue;
      if(strlen(mParsedFunctions[i].owner()) > 0)
        return mParsedFunctions[i].owner();
      return mParsedFunctions[i].name();
    }
    return "";
  }
  else if(s->type() == KLSymbol::Type_bracket2)
  {
    int bracketCount = 1;
    while(getPrevKLSymbol(s))
    {
      if(s->type() == KLSymbol::Type_bracket2)
        bracketCount++;
      else if(s->type() == KLSymbol::Type_bracket1)
        bracketCount--;
      if(bracketCount == 0)
        break;
    }
    std::string arrayType = getKLTypeForSymbol(s->prev());
    size_t bracketPos = arrayType.find('[');
    if(bracketPos != std::string::npos)
      arrayType = arrayType.substr(0, bracketPos);

    std::map<std::string,bool>::iterator it = gStrings.find(arrayType);
    if(it == gStrings.end())
    {
      gStrings.insert(std::pair<std::string, bool>(arrayType, false));
      it = gStrings.find(arrayType);
    }
    return it->first.c_str();
  }
  else if(s->type() == KLSymbol::Type_brace2)
  {
    int braceCount = 1;
    while(getPrevKLSymbol(s))
    {
      if(s->type() == KLSymbol::Type_brace2)
        braceCount++;
      else if(s->type() == KLSymbol::Type_brace1)
        braceCount--;
      if(braceCount == 0)
        break;
    }
    const KLSymbol * funcName = s->prev();
    if(!funcName)
      return "";
    if(funcName->prev())
    {
      if(funcName->prev()->type() == KLSymbol::Type_period)
      {
        const KLSymbol * funcOwner = funcName->prev()->prev();
        if(!funcOwner)
          return "";
        const char * funcOwnerType = getKLTypeForSymbol(funcOwner);
        if(strlen(funcOwnerType) == 0)
          return "";
        return getKLTypeForMemberOrMethod(funcOwnerType, funcName->str());
      }
    }
    return getKLTypeForMemberOrMethod("", funcName->str());
  }
  else if(s->type() != KLSymbol::Type_name)
    return "";
  const KLSymbol * p = s->prev();
  if(p)
  {
    if(p->type() == KLSymbol::Type_period)
    {
      const char * owner = getKLTypeForSymbol(p->prev());
      if(strlen(owner) == 0)
        return owner;
      return getKLTypeForMemberOrMethod(owner, s->str().c_str());
    }
    if(p->type() == KLSymbol::Type_bracket2)
      return getKLTypeForSymbol(p->prev());
    if(p->type() == KLSymbol::Type_brace2)
      return getKLTypeForSymbol(p->prev());
  }
  for(int i = mParsedVariables.size()-1;i>=0;i--)
  {
    if(mParsedVariables[i].symbol()->pos() > s->pos())
      continue;
    if(mParsedVariables[i].name() == name)
      return mParsedVariables[i].type();
  }
  return "";
}

const char * KLParserImpl::getKLTypeForMemberOrMethod(const std::string & owner, const std::string & member)
{
  std::map<std::string, KLParserImplPtr>::iterator it = sParsers.begin();
  for(;it!=sParsers.end();it++)
  {
    KLParserImplPtr parser = it->second;
    if(!parser)
      continue;
    for(size_t i=0;i<parser->mParsedFunctions.size();i++)
    {
      if(parser->mParsedFunctions[i].owner() == owner && parser->mParsedFunctions[i].name() == member)
        return parser->mParsedFunctions[i].type();
    }    
    for(size_t i=0;i<parser->mParsedStructs.size();i++)
    {
      if(parser->mParsedStructs[i].name() == owner)
      {
        for(unsigned int j=0;j<parser->mParsedStructs[i].nbMembers();j++)
        {
          if(parser->mParsedStructs[i].memberName(j) == member)
            return parser->mParsedStructs[i].memberType(j);
        }
      }
    }    
  }
  return "";
}
