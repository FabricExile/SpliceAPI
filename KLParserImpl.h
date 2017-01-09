// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __FabricSpliceImpl_KLPARSERIMPL_H__
#define __FabricSpliceImpl_KLPARSERIMPL_H__

#include <string>
#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>

namespace FabricSpliceImpl
{
  class KLParserImpl;
  typedef boost::shared_ptr<KLParserImpl> KLParserImplPtr;

  class KLParserImpl
  {
  public:

    class KLSymbol
    {
      friend class KLParserImpl;
      friend class KLConstant;

    public:

      enum Type {
        Type_none,

        Type_separator,
        Type_semicolon,
        Type_comma,
        Type_period,
        Type_comment,

        Type_firstkeyword,
        Type_in,
        Type_io,
        Type_if,
        Type_else,
        Type_switch,
        Type_case,
        Type_default,
        Type_for,
        Type_while,
        Type_do,
        Type_break,
        Type_continue,
        Type_this,
        Type_alias,
        Type_require,
        Type_return,
        Type_const,
        Type_null,
        Type_inline,
        Type_function,
        Type_operator,
        Type_struct,
        Type_object,
        Type_interface,
        Type_lastkeyword,

        Type_rt,
        Type_name,
        Type_number,
        Type_string,
        
        Type_assignment,
        Type_arithmetic,
        Type_brace1,
        Type_brace2,
        Type_bracket1,
        Type_bracket2,
        Type_curly1,
        Type_curly2,
        Type_pex1,
        Type_pex2,

        Type_maxtypes
      };

      unsigned int index() const;
      unsigned int pos() const { return mPos; }
      unsigned int length() const { return mLength; }
      Type type() const { return mType; }
      bool isKeyword() const;
      bool isFunction() const;
      const char *  typeName() const;
      static const char *  typeNameFromType(Type index);
      char front(unsigned int index = 0) const;
      char back(unsigned int index = 0) const;
      std::string str() const;
      bool contains(char c) const;
      unsigned int find(char c) const;
      const KLSymbol * prev(bool skipComments = true, unsigned int offset = 1) const;
      const KLSymbol * next(bool skipComments = true, unsigned int offset = 1) const;
      const char * parser() const;

    private:
      KLSymbol(const KLParserImpl * parser, const std::string * ptr, unsigned int pos);
      void setType(Type t) { mType = t; }
      bool merge(const KLSymbol & other);
      const KLParserImpl * mParser;
      const char * mPtr;
      const std::string * mStr;
      unsigned int mPos;
      unsigned int mLength;
      Type mType;
    };

    class KLConstant
    {
      friend class KLParserImpl;
    public:
      const KLSymbol * symbol() const { return mKLSymbol; }
      const char * comments() const { return mComments.c_str(); }
      const char * type() const { return mType.c_str(); }
      const char * name() const { return mName.c_str(); }
      const char * value() const { return mValue.c_str(); }

    private:
      KLConstant(const KLSymbol *& s);
      bool isValid() const { return mType.length() > 0 && mName.length() > 0 && mValue.length() > 0; }
      const KLSymbol * mKLSymbol;
      std::string mComments;
      std::string mType;
      std::string mName;
      std::string mValue;
    };

    class KLVariable
    {
      friend class KLParserImpl;
    public:
      const KLSymbol * symbol() const { return mKLSymbol; }
      const char * type() const { return mType.c_str(); }
      const char * name() const { return mName.c_str(); }

    private:
      KLVariable(const KLSymbol * s, const KLSymbol * t);
      bool isValid() const { return mType.length() > 0 && mName.length(); }
      const KLSymbol * mKLSymbol;
      std::string mType;
      std::string mName;
    };

    class KLStruct
    {
      friend class KLParserImpl;
    public:
      const KLSymbol * symbol() const { return mKLSymbol; }
      const char * comments() const { return mComments.c_str(); }
      const char * type() const { return mType.c_str(); }
      const char * name() const { return mName.c_str(); }
      unsigned int nbInterfaces() const { return mInterfaces.size(); }
      const char * interface(unsigned int index) const;
      unsigned int nbMembers() const { return mMemberNames.size(); }
      const char * memberType(unsigned int index) const;
      const char * memberName(unsigned int index) const;

    private:
      KLStruct(const KLSymbol *& s);
      bool isValid() const { return mName.length() > 0 && mMemberNames.size(); }
      const KLSymbol * mKLSymbol;
      std::string mComments;
      std::string mType;
      std::string mName;
      std::vector<std::string> mInterfaces;
      std::vector<std::string> mMemberTypes;
      std::vector<std::string> mMemberNames;
    };

    class KLArgumentList
    {
      friend class KLOperator;
      friend class KLFunction;
      friend class KLParserImpl;
    public:
      unsigned int nbArgs() const { return mModes.size(); }
      const char * mode(unsigned int index) const;
      const char * type(unsigned int index) const;
      const char * name(unsigned int index) const;

    private:
      KLArgumentList() {}
      KLArgumentList(const KLSymbol *& s);
      bool isValid() const { return mModes.size(); }
      std::vector<std::string> mModes;
      std::vector<std::string> mTypes;
      std::vector<std::string> mNames;
    };

    class KLOperator
    {
      friend class KLParserImpl;
    public:
      const KLSymbol * symbol() const { return mKLSymbol; }
      const char * comments() const { return mComments.c_str(); }
      const char * name() const { return mName.c_str(); }
      bool isPex() const { return mPexArg.length() > 0; }
      const char * pexArgument()  const { return mPexArg.c_str(); }
      const KLArgumentList * arguments() const { return &mArguments; }
      const KLSymbol * bodyStart() const { return mBody1; }
      const KLSymbol * bodyEnd() const { return mBody2; }

    private:
      KLOperator(const KLSymbol *& s);
      bool isValid() const { return mName.length(); }
      const KLSymbol * mKLSymbol;
      std::string mComments;
      std::string mName;
      std::string mPexArg;
      KLArgumentList mArguments;
      const KLSymbol * mBody1;
      const KLSymbol * mBody2;
    };    

    class KLFunction
    {
      friend class KLParserImpl;
    public:
      const KLSymbol * symbol() const { return mKLSymbol; }
      const char * comments() const { return mComments.c_str(); }
      const char * type() const { return mType.c_str(); }
      const char * owner() const { return mOwner.c_str(); }
      const char * name() const { return mName.c_str(); }
      const KLArgumentList * arguments() const { return &mArguments; }
      const KLSymbol * bodyStart() const { return mBody1; }
      const KLSymbol * bodyEnd() const { return mBody2; }

    private:
      KLFunction(const KLSymbol *& s, bool requireFunctionKeyWord);
      bool isValid() const { return mName.length(); }
      const KLSymbol * mKLSymbol;
      std::string mComments;
      std::string mType;
      std::string mOwner;
      std::string mName;
      KLArgumentList mArguments;
      const KLSymbol * mBody1;
      const KLSymbol * mBody2;
    };   

    class KLInterface
    {
      friend class KLParserImpl;
    public:
      const KLSymbol * symbol() const { return mKLSymbol; }
      const char * comments() const { return mComments.c_str(); }
      const char * name() const { return mName.c_str(); }
      unsigned int nbFunctions() const { return mFunctions.size(); }
      const KLFunction * function(unsigned int index) const;

    private:
      KLInterface(const KLSymbol *& s);
      bool isValid() const { return mName.length() > 0 && mFunctions.size(); }
      const KLSymbol * mKLSymbol;
      std::string mComments;
      std::string mName;
      std::vector<KLFunction> mFunctions;
    };

    ~KLParserImpl();

    const char * owner() const { return mOwner.c_str(); }
    const char * name() const { return mName.c_str(); }
    const char * code() const { return mStr.c_str(); }
    static unsigned int getNbParsers();
    static KLParserImplPtr getParser(unsigned int index);
    static KLParserImplPtr getParser(const char * owner, const char * name, const char * klCode);
    static void resetAll();
    
    // update parsed code
    bool parse(const char * klCode);

    // basic accessors
    unsigned int nbKLSymbols() const { return (unsigned int)mKLSymbols.size(); }
    const KLSymbol * getKLSymbol(unsigned int symbolIndex) const;
    const KLSymbol * getKLSymbolFromCharIndex(unsigned int charIndex) const;
    bool getNextKLSymbol(const KLSymbol *& symbol, bool skipComments = true, unsigned int offset = 1) const;
    bool getPrevKLSymbol(const KLSymbol *& symbol, bool skipComments = true, unsigned int offset = 1) const;
    bool getNextKLSymbolOfType(const KLSymbol *& symbol, KLSymbol::Type type) const;
    bool getPrevKLSymbolOfType(const KLSymbol *& symbol, KLSymbol::Type type) const;

    // access to parsed symbols
    unsigned int getNbKLRequires() const { return (unsigned int)mParsedRequires.size(); }
    const char * getKLRequire(unsigned int index) const;
    unsigned int getNbKLConstants() const { return (unsigned int)mParsedConstants.size(); }
    const KLConstant * getKLConstant(unsigned int index) const;
    unsigned int getNbKLVariables() const { return (unsigned int)mParsedVariables.size(); }
    const KLVariable * getKLVariable(unsigned int index) const;
    unsigned int getNbKLInterfaces() const { return (unsigned int)mParsedInterfaces.size(); }
    const KLInterface * getKLInterface(unsigned int index) const;
    unsigned int getNbKLStructs() const { return (unsigned int)mParsedStructs.size(); }
    const KLStruct * getKLStruct(unsigned int index) const;
    unsigned int getNbKLOperators() const { return (unsigned int)mParsedOperators.size(); }
    const KLOperator * getKLOperator(unsigned int index) const;
    unsigned int getNbKLFunctions() const { return (unsigned int)mParsedFunctions.size(); }
    const KLFunction * getKLFunction(unsigned int index) const;

    // code completion functions
    const char * getKLTypeForSymbol(const KLSymbol * s) const;
    static const char * getKLTypeForMemberOrMethod(const std::string & owner, const std::string & member);

  private:
    KLParserImpl(const char * owner, const char * name, const char * klCode);
    unsigned int getIndexOfKLSymbol(const KLSymbol * symbol) const;
    std::string getCommentsPriorToKLSymbol(const KLSymbol * s) const;
    std::string mOwner;
    std::string mName;
    const char * mPtr;
    std::string mStr;
    std::vector<KLSymbol> mKLSymbols;
    std::vector<std::string> mParsedRequires;
    std::vector<KLConstant> mParsedConstants;
    std::vector<KLVariable> mParsedVariables;
    std::vector<KLInterface> mParsedInterfaces;
    std::vector<KLStruct> mParsedStructs;
    std::vector<KLOperator> mParsedOperators;
    std::vector<KLFunction> mParsedFunctions;

    static std::map<std::string, KLParserImplPtr> sParsers;
  };
};

#endif
