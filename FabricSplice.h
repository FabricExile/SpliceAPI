// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

// NOTE: The Fabric Splice API will be deprecated in 2.0. 
// For new integrations please just use the FabricCore API 
// and Fabric Services DFGWrapper.

#ifndef __FabricSplice_H__
#define __FabricSplice_H__

/*SPHINX:index

.. _SPLICECAPI:

|FABRIC_PRODUCT_NAME| Splice C/C++ API Documentation
=======================================================

.. image:: /images/Splice/Splice_logo.png
   :width: 360px
   :height: 64px

| |FABRIC_PRODUCT_NAME| version |FABRIC_VERSION|
| |FABRIC_COPYRIGHT|

.. note:: The SpliceAPI will be deprecated in Fabric Engine 2.0.

Table of Contents
-----------------

.. toctree::
  :maxdepth: 2
  
  intro
  helloworld
  functions
  classes

Indices and Tables
------------------

* :ref:`genindex`
* :ref:`search`

*/

/*SPHINX:intro

Introduction
============

.. note:: You can find the source code for the SpliceAPI here: http://github.com/fabric-engine/SpliceAPI

The Fabric:Splice C/C++ API, referred to as :dfn:`SPLICECAPI` in this document, is an abstraction layer to the `Fabric Core CAPI <http://documentation.fabric-engine.com/CreationPlatform/latest/HTML/CAPIProgrammingGuide/index.html>`_. The SPLICEAPI provides a simpler way to integrate Fabric Core based functionality into C/C++ hosts applications and wraps lots of the rudimentary facilities. The :dfn:`SPLICECAPI` also supports additional facilities, such as persistence, for example.

Aside from this documentation API you can find a general description of how to integrate Splice into a host application here: :ref:`SPLICEINTEGRATION`. This also includes recommendations for build settings and C defines.

One API, Two Interfaces
-----------------------

:dfn:`SPLICECAPI` is implemented as a pure C API with a thin, inlined C++ interface that makes it easier to use in C++ applications.  This is done to minimize linking issues, as the C linking interface on the platforms that Fabric:Splice supports is much more controlled than the C++ linking interface. The C++ interface is thus purely a C++ programmer convenience; however, it is a big programmer convenience, and as such it is recommended that you use the C++ language interface when possible.  Both interfaces link with exactly the same shared library (DLL).

Since the host application targeted with the :dfn:`SPLICECAPI` are mainly C++ based, when discussing the API in this guide, we will only present the C++ version.

*/
/*SPHINX:helloworld

Hello World
============

The sample below shows the most basic use of the SPLICECAPI. This can be used to test the library and establish the right linker settings etc.

.. code-block:: c++

    #include <FabricSplice.h>

    using namespace FabricSplice;

    int main( int argc, const char* argv[] )
    {
      Initialize();

      // create a graph
      DGGraph graph = DGGraph("myGraph");

      // construct a single DG node
      graph.constructDGNode();

      // create an operator
      std::string klCode = "";
      klCode += "operator helloWorldOp() {\n";
      klCode += "  report('Hello World from KL!');\n";
      klCode += "}\n";
      graph.constructKLOperator("helloWorldOp", klCode.c_str());

      // // evaluate the graph
      graph.evaluate();

      Finalize();
      
      return 0;
    }
*/
/*SPHINX:classes

.. _classes:

Classes
========================

All Fabric:Splice C++ classes are implemented within the :dfn:`FabricSplice` namespace.

.. toctree::
  :maxdepth: 1
  
  logging
  exception
  dggraph
  dgport
  scenemanagement
  scripting
  klparser

*/
/*SPHINX:functions

.. _functions:

Global functions 
=========================

The SPLICECAPI provides several global functions. These can be used to initialize the use of the library as well as control the licensing aspects of Fabric:Splice.

.. code-block:: c++

    namespace FabricSplice 
    {
      // Initialize needs to be called before calling into 
      // any other functionality of the SPLICECAPI.
      void Initialize();

      // Finalize needs to be called whenever the host application
      // is finished with accessing the SPLICECAPI.
      void Finalize();

      // Returns the major component of the Fabric version
      uint8_t GetFabricVersionMaj();

      // Returns the minor component of the Fabric version
      uint8_t GetFabricVersionMin();

      // Returns the revision component of the Fabric version
      uint8_t GetFabricVersionRev();

      // Returns the Core Version number as a string
      char const * GetFabricVersionVersionStr();

      // Returns the Splice Version number as a string
      char const * GetSpliceVersion();

      // Constructs a FabricCore client
      FabricCore::Client ConstructClient(int guarded = 1, FabricCore::ClientOptimizationType optType = FabricCore::ClientOptimizationType_Background);

      // Destroys a FabricCore client
      bool DestroyClient(bool force = false);

      // returns the ID for the client-context used by splice
      char const * GetClientContextID();

      // addExtFolder can be used to add paths to KL extension
      // resolval mechanism. First the folders listed in the
      // FABRIC_EXTS_PATH variable will be checked, followed
      // by the list of folders defined through this function.
      bool addExtFolder(const char * folder)

      // a DCC callback function to gather KL operator code from the UI
      typedef const char *(*GetOperatorSourceCodeFunc)(const char * graphName, const char * opName);

      // set a callback to allow the splice persistence framework to gather
      // the last unsaved code for a given KL operator. this code might still
      // sit in the UI somewhere but hasn't been pushed to the DGGraph.
      void setDCCOperatorSourceCodeCallback(GetOperatorSourceCodeFunc func);

      // creates a RTVal just given a KL type name
      FabricCore::RTVal constructRTVal(const char * rt);

      // creates a RTVal given a KL type name and construction args
      FabricCore::RTVal constructRTVal(const char * rt, uint32_t nbArgs, const FabricCore::RTVal * args);

      // creates a RTVal just given a KL object name
      FabricCore::RTVal constructObjectRTVal(const char * rt);

      // creates a RTVal given a KL object name and construction args
      FabricCore::RTVal constructObjectRTVal(const char * rt, uint32_t nbArgs, const FabricCore::RTVal * args);

      // creates a KL interface RTVal given a KL object to cast
      FabricCore::RTVal constructInterfaceRTVal(const char * rt, const FabricCore::RTVal & object);

      // creates a Boolean RTVal given its value
      FabricCore::RTVal constructBooleanRTVal(bool value);

      // creates a SInt8 RTVal given its value
      FabricCore::RTVal constructSInt8RTVal(int8_t value);

      // creates a SInt16 RTVal given its value
      FabricCore::RTVal constructSInt16RTVal(int16_t value);

      // creates a SInt32 RTVal given its value
      FabricCore::RTVal constructSInt32RTVal(int32_t value);

      // creates a SInt64 RTVal given its value
      FabricCore::RTVal constructSInt64RTVal(int64_t value);

      // creates a UInt8 RTVal given its value
      FabricCore::RTVal constructUInt8RTVal(uint8_t value);

      // creates a UInt16 RTVal given its value
      FabricCore::RTVal constructUInt16RTVal(uint16_t value);

      // creates a UInt32 RTVal given its value
      FabricCore::RTVal constructUInt32RTVal(uint32_t value);

      // creates a UInt64 RTVal given its value
      FabricCore::RTVal constructUInt64RTVal(uint64_t value);

      // creates a Float32 RTVal given its value
      FabricCore::RTVal constructFloat32RTVal(float value);

      // creates a Float64 RTVal given its value
      FabricCore::RTVal constructFloat64RTVal(double value);

      // creates a Data RTVal given its value
      FabricCore::RTVal constructDataRTVal(void *value);

      // creates a String RTVal given its value
      FabricCore::RTVal constructStringRTVal(const char * value);

      // creates a variable array RTVal given its type
      FabricCore::RTVal constructVariableArrayRTVal(const char * rt);

      // creates an external array RTVal given its type, nbElements and the void pointer
      FabricCore::RTVal constructExternalArrayRTVal(const char * rt, uint32_t nbElements, void * data);
    };
*/
/*SPHINX:logging

.. _logging:

FabricSplice::Logging
=========================

The Logging class provides static methods to redirect the log outputs of the SPLICECAPI. This includes output for KL report statements, KL queueStatusMessage statements as well as KL errors and compiler errors. Furthermore this class provides helper functions for profiling, measuring time and getting access to internal timers. Please refer to the 08_profiling sample in the api samples for an example of that.

Example
---------------------------------

The sample below shows how to use log redirection.

.. code-block:: c++

    #include <FabricSplice.h>

    using namespace FabricSplice;

    void myLogFunc(const char * message, unsigned int length)
    {
      printf("[MyCallback] %s\n", message);
    }

    void myLogErrorFunc(const char * message, unsigned int length)
    {
      printf("[MyCallback] Error: %s\n", message);
    }

    void myCompilerErrorFunc(
      unsigned int row, 
      unsigned int col, 
      const char * file,
      const char * level,
      const char * desc
    ) {
      printf("[MyCallback] KL Error: %s, Line %d, Col %d: %s\n", file, (int)row, (int)col, desc);
    }

    void myKLReportFunc(const char * message, unsigned int length)
    {
      printf("[MyCallback] KL Reports: %s\n", message);
    }

    void myKLStatusFunc(const char * topic, unsigned int topicLength,  const char * message, unsigned int messageLength)
    {
      printf("[MyCallback] KL Status for '%s': %s\n", topic, message);
    }

    int main( int argc, const char* argv[] )
    {
      Initialize();

      // setup the callback functions
      Logging::setLogFunc(myLogFunc);
      Logging::setLogErrorFunc(myLogErrorFunc);
      Logging::setCompilerErrorFunc(myCompilerErrorFunc);
      Logging::setKLReportFunc(myKLReportFunc);
      Logging::setKLStatusFunc(myKLStatusFunc);

      // create a graph
      DGGraph graph = DGGraph("myGraph");

      // create a DG node
      graph.constructDGNode();

      // create a member
      graph.addDGNodeMember("value", "Vec3");

      // create a port
      DGPort port = graph.addDGPort("value", "value", Port_Mode_IO);

      // create an op
      graph.constructKLOperator("testOp");

      // on purpose create a compiler error
      try
      {
        graph.setKLOperatorSourceCode("testOp", "operator testOp(Vec3 value) {adsadsd;}");
      }
      catch(Exception e)
      {
        printf("Caught error: %s\n", e.what());
      }

      // update the operator to report from KL
      graph.setKLOperatorSourceCode("testOp", "operator testOp() { report('my message');}");

      // evaluate will invoke all operators
      // and in this case also call the myKLReportFunc
      graph.evaluate();

      // update the operator to send a status update from KL
      graph.setKLOperatorSourceCode("testOp", "operator testOp() { queueStatusMessage('info', 'nothing going on!');}");

      // evaluate will invoke all operators
      // and in this case also call the myKLStatusFunc
      graph.evaluate();

      Finalize();
      return 0;
    }

Class Outline
---------------------------------

.. code-block:: c++

    namespace FabricSplice
    {
      typedef void(*LoggingFunc)(const char * message, unsigned int messageLength);
      typedef void(*CompilerErrorFunc)(unsigned int row, unsigned int col, const char * file, const char * level, const char * desc);
      typedef void(*StatusFunc)(const char * topic, unsigned int topicLength, const char * message, unsigned int messageLength);

      class Logging
      {
      public:

        // sets the callback for generic log messages
        static void setLogFunc(LoggingFunc func);

        // sets the callback for error log messages
        static void setLogErrorFunc(LoggingFunc func);

        // sets the callback for KL compiler error messages
        static void setCompilerErrorFunc(CompilerErrorFunc func);

        // sets the callback for KL report statements
        static void setKLReportFunc(LoggingFunc func);

        // sets the callback for KL queueStatusMessage statements
        static void setKLStatusFunc(StatusFunc func);

        // enable timers
        static void enableTimers();

        // disable timers
        static void disableTimers();

        // reset a timer
        static void resetTimer(const char * name);

        // start a timer
        static void startTimer(const char * name);

        // stop a timer and accumulate the time
        static void stopTimer(const char * name);

        // log a given timer
        static void logTimer(const char * name);      

        // return the number of existing timers
        static unsigned int getNbTimers();

        // return the name of a specific timer
        char const * getTimerName(unsigned int index);

        // a timer which records time on construction and destruction
        class AutoTimer
        {
        public:
          AutoTimer(std::string const &name);
          ~AutoTimer();
          const char * getName();
          void resume();
          void stop();

        private:
          std::string mName;
        };        
      };
    };
*/
/*SPHINX:klparser

.. _klparser:

FabricSplice::KLParser
=========================

The KLParser class provides functionality to parse KL files. It also implements sub-classes for accessing contextual symbols for constants, structs, operators and functions. Using the :ref:`klparser` you can implement code completion tools, analytic tools for KL code, doxygen style documentation generation tools etc.

.. note:: The KLParser will be deprecated after 1.13.0 and replaced by a proper AST representation.

Example
---------------------------------

The sample below shows how to use the :ref:`klparser` for iterating over all defined symbols within a KL file.

.. code-block:: c++

    #include <FabricSplice.h>

    using namespace FabricSplice;

    int main( int argc, const char* argv[] )
    {
      std::string klCode;
      klCode += "struct MyType {\n";
      klCode += "  Float32 x;\n";
      klCode += "  Float32 y;\n";
      klCode += "}\n";
      klCode += "\n";
      klCode += "function MyType(Float64 x, Float64 y) {\n";
      klCode += "  this.x = x;\n";
      klCode += "  this.y = y;\n";
      klCode += "}\n";

      KLParser parser = KLParser::getParser("MyType", "MyType", klCode.c_str());
      for(unsigned int j=0;j<parser.getNbKLSymbols();j++)
      {
        KLParser::KLSymbol s = parser.getKLSymbol(j);
        printf("%03d: '%s' '%s'\n", (int)i, s.typeName(), s.str().c_str());
      }
      return 0;
    }

Class Outline
---------------------------------

.. code-block:: c++

    namespace FabricSplice
    {
      class KLParser
      {
      public:

        class KLSymbol
        {
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

        public:

          // copy constructor
          KLSymbol(KLSymbol const & other);

          // copy operator
          KLSymbol & operator =( KLSymbol const & other );

          // returns true if the object is valid
          bool isValid() const;

          // bool conversion operator
          operator bool() const;

          // returns the index of this symbol
          unsigned int index() const;

          // returns the char position within the code
          unsigned int pos() const;

          // returns the length of this symbol
          unsigned int length() const;

          // returns the type of this symbol
          Type type() const;

          // returns true of this symbol is a keyword
          bool isKeyword() const;

          // returns
          const char * typeName() const;

          // returns a char from the front (given an index);
          char front(unsigned int index = 0) const;

          // returns a char from the back (given an index);
          char back(unsigned int index = 0) const;

          // returns the contained string
          std::string str() const;

          // returns true if this contains a given char
          bool contains(char c) const;

          // returns the index of a given char, or UINT_MAX if not found
          unsigned int find(char c) const;

          // returns the previous symbol before this one, or an invalid one
          KLSymbol prev(bool skipComments = true, unsigned int offset = 1) const;

          // returns the next symbol after this one, or an invalid one
          KLSymbol next(bool skipComments = true, unsigned int offset = 1) const;

          // returns the name of the parser this symbol belongs to
          const char * parser() const;
        };

        class KLConstant
        {
        public:
          // copy constructor
          KLConstant(KLConstant const & other);

          // copy operator
          KLConstant & operator =( KLConstant const & other );

          // returns true if the object is valid
          bool isValid() const;

          // bool conversion operator
          operator bool() const;

          // returns the symbol of this KLConstant
          KLSymbol symbol() const;

          // returns the comments of this KLConstant
          const char * comments() const;

          // returns the type of this KLConstant
          const char * type() const;

          // returns the name of this KLConstant
          const char * name() const;

          // returns the value of this KLConstant
          const char * value() const;
        };

        class KLVariable
        {
        public:
          // copy constructor
          KLVariable(KLVariable const & other);

          // copy operator
          KLVariable & operator =( KLVariable const & other );

          // returns true if the object is valid
          bool isValid() const;

          // bool conversion operator
          operator bool() const;

          // returns the symbol of this KLVariable
          KLSymbol symbol() const;

          // returns the type of this KLVariable
          const char * type() const;

          // returns the name of this KLVariable
          const char * name() const;
        };

        class KLStruct
        {
        public:
          // copy constructor
          KLStruct(KLStruct const & other);

          // copy operator
          KLStruct & operator =( KLStruct const & other );

          // returns true if the object is valid
          bool isValid() const;

          // bool conversion operator
          operator bool() const;

          // returns the symbol of this KLStruct
          KLSymbol symbol() const;

          // returns the comments of this KLStruct
          const char * comments() const;

          // returns the type of this KLStruct
          const char * type() const;

          // returns the name of this KLStruct
          const char * name() const;

          // returns number of interfaces in this KLStruct
          unsigned int nbInterfaces() const;

          // returns name of an interface of a given index
          const char * getInterface(unsigned int index) const;

          // returns number of members in this KLStruct
          unsigned int nbMembers() const;

          // returns the type of a member of this KLStruct with a given index
          const char * memberType(unsigned int index) const;

          // returns the type of a member of this KLStruct with a given index
          const char * memberName(unsigned int index) const;
        };

        class KLArgumentList
        {
        public:
          // copy constructor
          KLArgumentList(KLArgumentList const & other);

          // copy operator
          KLArgumentList & operator =( KLArgumentList const & other );

          // returns true if the object is valid
          bool isValid() const;

          // bool conversion operator
          operator bool() const;

          // returns number of arguments in this KLArgumentList
          unsigned int nbArgs() const;

          // returns the mode of an argument with a given index
          const char * mode(unsigned int index) const;

          // returns the type of an argument with a given index
          const char * type(unsigned int index) const;

          // returns the name of an argument with a given index
          const char * name(unsigned int index) const;
        };

        class KLOperator
        {
        public:
          // copy constructor
          KLOperator(KLOperator const & other);

          // copy operator
          KLOperator & operator =( KLOperator const & other );

          // returns true if the object is valid
          bool isValid() const;

          // bool conversion operator
          operator bool() const;

          // returns the symbol of this KLOperator
          KLSymbol symbol() const;

          // returns the comments of this KLOperator
          const char * comments() const;

          // returns the name of this KLOperator
          const char * name() const;

          // returns true if this KLOperator uses a PEX notation
          bool isPex() const;

          // returns the pexArgument of this KLOperator
          const char * pexArgument() const;

          // returns the argument list of this KLOperator
          KLArgumentList arguments() const;

          // returns the symbol starting the body of this KLOperator
          KLSymbol bodyStart() const;

          // returns the symbol ending the body of this KLOperator
          KLSymbol bodyEnd() const;
        };

        class KLFunction
        {
        public:
          // copy constructor
          KLFunction(KLFunction const & other);

          // copy operator
          KLFunction & operator =( KLFunction const & other );

          // returns true if the object is valid
          bool isValid() const;

          // bool conversion operator
          operator bool() const;

          // returns the symbol of this KLFunction
          KLSymbol symbol() const;

          // returns the comments of this KLFunction
          const char * comments() const;

          // returns the type of this KLFunction (or "" if it is a void);
          const char * type() const;

          // returns the name of this KLFunction
          const char * name() const;

          // returns the owner of this KLFunction (or "" if it's not a method);
          const char * owner() const;

          // returns the argument list of this KLFunction
          KLArgumentList arguments() const;

          // returns the symbol starting the body of this KLFunction
          KLSymbol bodyStart() const;

          // returns the symbol ending the body of this KLFunction
          KLSymbol bodyEnd() const;
        };

        class KLInterface
        {
        public:
          // returns the symbol of this KLInterface
          KLSymbol symbol() const;

          // returns the comments of this KLInterface
          const char * comments() const;

          // returns the name of this KLInterface
          const char * name() const;

          // returns the number of functions on this KLInterface
          unsigned int nbFunctions() const;

          // returns a function of this KLInterface by index
          KLFunction function(unsigned int index) const;
        };

        // default constructor
        KLParser();

        // copy constructor
        KLParser(KLParser const & other);

        // copy operator
        KLParser & operator =( KLParser const & other );

        // default destructor
        ~KLParser();

        // returns true if the object is valid
        bool isValid() const;

        // bool conversion operator
        operator bool() const;

        // returns the owner of the parser
        const char * owner() const ;

        // returns the name of the parser
        const char * name() const ;

        // returns the contained sourcecode of the parser
        const char * code() const ;

        // returns the number of current parsers
        static unsigned int getNbParsers();

        // returns a parser given an index
        static KLParser getParser(unsigned int index);

        // returns a parser given a name an optional klCode
        // if the parser doesn't exist yet - it will be created.
        static KLParser getParser(const char * owner, const char * name, const char * klCode = NULL);

        // parse new KL code in this parser
        bool parse(const char * klCode);

        // returns the number of KL symbols
        unsigned int getNbKLSymbols() const;

        // returns a specific KL symbol
        KLSymbol getKLSymbol(unsigned int symbolIndex) const;

        // returns the symbol for a special character index in the KL code
        KLSymbol getKLSymbolFromCharIndex(unsigned int charIndex) const;

        // returns the number of required KL types / extensions
        unsigned int getNbKLRequires() const;

        // returns the name of the required KL type / extension given an index
        const char * getKLRequire(unsigned int index) const;

        // returns the number of KL constants
        unsigned int getNbKLConstants() const;

        // returns the KL constant given an index
        KLConstant getKLConstant(unsigned int index) const;

        // returns the number of KL variables
        unsigned int getNbKLVariables() const;

        // returns the KL variable given an index
        KLVariable getKLVariable(unsigned int index) const;

        // returns the number of KL interfaces
        unsigned int getNbKLInterfaces() const;
        
        // returns the KL interface given an index
        KLInterface getKLInterface(unsigned int index) const;

        // returns the number of KL structs / objects
        unsigned int getNbKLStructs() const;

        // returns the KL struct / object given an index
        KLStruct getKLStruct(unsigned int index) const;

        // returns the number of KL operators
        unsigned int getNbKLOperators() const;

        // returns the KL operator given an index
        KLOperator getKLOperator(unsigned int index) const;

        // returns the number of KL functions / methods
        unsigned int getNbKLFunctions() const;

        // returns the KL function given an index
        KLFunction getKLFunction(unsigned int index) const;

        // returns the KL type for a given name symbol (or "" if unknown)
        const char * getKLTypeForSymbol(const KLSymbol & symbol) const;

        // returns the KL type for a member or method below a given owner type
        static const char * getKLTypeForSymbol(const char * owner, const char * member);
      };
    };
*/
/*SPHINX:scenemanagement

.. _scenemanagement:

FabricSplice::SceneManagement
===============================

The SceneManagement class provides static methods to perform certain scene management related tasks, such as draw all drawable :ref:`dgport` object, as well deal with interactive manipulation.

Class Outline
---------------------------------

.. code-block:: c++

    namespace FabricSplice
    {
      struct ManipulationData {
        int event;
        void * userData;
        const char * graphName;
        const char * portName;
        const FabricCore::RTVal * manipulationContext;
        const FabricCore::RTVal * manipulationResult;
      };

      typedef int(*ManipulationFunc)(ManipulationData * data);

      class SceneManagement
      {
      public:

        // sets the callback for manipulation
        static void setManipulationFunc(ManipulationFunc func);

        // returns true if there is anything to render
        static bool hasRenderableContent();

        // draw all drawable ports
        // ensure to only call this with a valid
        // OpenGL context set, otherwise it might
        // cause instabilities
        static void drawOpenGL(FabricCore::RTVal & drawContext);

        // raycast against all raycastable objects
        static bool raycast(FabricCore::RTVal & raycastContext, DGPort & port);
      };
    };
*/
/*SPHINX:scripting

.. _scripting:

FabricSplice::Scripting
===============================

The Scripting class provides static methods for parsing script arguments. This can be useful within Splice integrations. Options are provided as json strings typically, and the following helper functions simplify parsing these.

Class Outline
---------------------------------

.. code-block:: c++

    namespace FabricSplice
    {
      class Scripting
      {
      public:

        // decodes a flat list of scripting arguments into a dictionary of 
        // argument values. this also assume one of the arguments (index 1 or 2)
        // to contain a json structure with additional values
        static FabricCore::Variant parseScriptingArguments(const char * action, const char * reference, const char * data, const char * auxiliary);

        // returns a bool argument of a given parsed dictionary
        static bool consumeBooleanArgument(FabricCore::Variant & argsDict, const char * name, bool defaultValue = false, bool optional = false);

        // returns a int argument of a given parsed dictionary
        static int consumeIntegerArgument(FabricCore::Variant & argsDict, const char * name, int defaultValue = 0, bool optional = false);

        // returns a float argument of a given parsed dictionary
        static float consumeScalarArgument(FabricCore::Variant & argsDict, const char * name, float defaultValue = 0.0, bool optional = false);

        // returns a string argument of a given parsed dictionary (string variant)
        static std::string consumeStringArgument(FabricCore::Variant & argsDict, const char * name, const char * defaultValue = "", bool optional = false);

        // returns a variant argument of a given parsed dictionary
        static FabricCore::Variant consumeVariantArgument(FabricCore::Variant & argsDict, const char * name, const FabricCore::Variant & defaultValue = FabricCore::Variant(), bool optional = false);
      };
    };
*/
/*SPHINX:exception

.. _exception:

FabricSplice::Exception
=========================

The Exception is used to catch any errors happening within Fabric:Splice or the Fabric Core from a host application. You may wrap any call to the :dfn:`SPLICECAPI` like this:

Example
---------------------------------

.. code-block:: c++

    try
    {
      ... call to SPLICECAPI ...
    }
    catch(FabricSplice::Exception e)
    {
      printf("Splice Error %s\n", e.what());
    }

It's recommended to wrap the try and catch calls into macros, so that you can deploy them easily anywhere you use the :dfn:`SPLICECAPI`.

Class Outline
---------------------------------

.. code-block:: c++

    namespace FabricSplice
    {
      class Exception
      {
      public:

        // returns the description of the Exception
        char const * what() const;
        
        // returns the description of the Exception
        operator const char *() const;
      };
    };
*/
/*SPHINX:dgport

.. _dgport:

FabricSplice::DGPort
=========================

The DGPort class allows to get or set :ref:`dggraph` data. DGPorts can communicate with the FabricCore layer in various ways.

.. note::
  
  When accessing data on instances of FabricSplice::DGPort, eg. through
  :code:`DGPort::getRTVal()`, it is the responsibility of the client to ensure
  that accesses to data on the same port are synchronized, ie. that two threads
  don't try to change the value at the same time.  However, it is safe to access
  data on different instances of FabricSplice::DFGPort without locking.
  
Example
---------------------------------

The sample below shows how to use ports. The class outline can be found at the end of this page.

.. code-block:: c++

    #include <FabricSplice.h>
    #include <vector>

    using namespace FabricSplice;

    int main( int argc, const char* argv[] )
    {
      Initialize();

      // create a graph
      DGGraph graph = DGGraph("myGraph");

      // create a DG node
      graph.constructDGNode();

      // create a member
      graph.addDGNodeMember("myScalar", "Scalar");

      // create a port
      // the mode defines if a port can be read from, 
      // written to or both (like in this case)
      DGPort port = graph.addDGPort("myPortName", "myScalar", Port_Mode_IO);

      // set some data on the port using JSON
      port.setJSON("1.3");

      // print the data
      printf("%s\n", port.getJSON().getStringData());

      // set some data on the port using a variant
      port.setVariant(FabricCore::Variant::CreateFloat64(2.6));

      // print the data
      printf("%s\n", port.getJSON().getStringData());

      // nodes can store any number of values. the count of the values
      // is referred to as 'slice count'
      port.setSliceCount(3);
      port.setVariant(FabricCore::Variant::CreateFloat64(1.6), 0);
      port.setVariant(FabricCore::Variant::CreateFloat64(2.6), 1);
      port.setVariant(FabricCore::Variant::CreateFloat64(3.6), 2);

      // print the data
      for(uint32_t i=0;i<port.getSliceCount();i++)
        printf("%x: %s\n", i, port.getJSON(i).getStringData());

      // aside from json and variants you can also use the 
      // high performance IO on a port.
      std::vector<float> values(5);
      values[0] = 3.4;
      values[1] = 4.5;
      values[2] = 5.6;
      values[3] = 6.7;
      values[4] = 7.8;
      port.setSliceCount(values.size());
      port.setAllSlicesData(&values[0], sizeof(float) * values.size());

      // clear the values
      values.clear();
      values.resize(port.getSliceCount());

      // get the data again using the high performance IO
      port.getAllSlicesData(&values[0], sizeof(float) * values.size());

      // print the retured data
      for(uint32_t i=0;i<values.size();i++)
        printf("%x: %f\n", i, values[i]);

      Finalize();
      return 0;
    }

Class Outline
---------------------------------

.. code-block:: c++

    namespace FabricSplice
    {
      enum Port_Mode
      {
        Port_Mode_IN = 0,
        Port_Mode_OUT = 1,
        Port_Mode_IO = 2
      };

      class DGPort
      {
      public:

        // default constructor
        DGPort();

        // copy constructor
        DGPort(DGPort const & other);

        // copy operator
        DGPort & operator =( DGPort const & other );

        // returns true if the object is valid
        bool isValid() const;

        // bool conversion operator
        operator bool() const;

        // resets the port and detaches it
        void clear();

        // returns the name of this DGPort
        char const * getName();

        // returns the name of the member this DGPort is connected to
        char const * getMember();

        // returns the name of the DGNode this DGPort is connected to
        char const * getDGNodeName();

        // returns a unique key descripting the DGPort
        char const * getKey();

        // returns the mode of this DGPort
        DGPort_Mode getMode();

        // sets the mode of this DGPort
        void setMode(DGPort_Mode mode);

        // returns the data type of the member this DGPort is connected to
        char const * getDataType();

        // returns the data size of a single element of the member this DGPort is connected to.
        // So for example, both for a 'Vec3' and 'Vec3[]' this will return sizeof(Vec3) == 12
        unsigned int getDataSize();

        // returns true if the data type of this DGPort is shallow.
        // only shallow data types can be used with the high performance IO
        bool isShallow();

        // returns true if the data type of this DGPort is an array (Vec3[] for example)
        bool isArray();

        // returns true if the data type of this DGPort is a struct
        bool isStruct();

        // returns true if the data type of this DGPort is an object
        bool isObject();

        // returns true if the data type of this DGPort is an interface
        bool isInterface();

        // returns true if this port auto initializes KL objects
        bool doesAutoInitObjects() const { return mAutoInitObjects; }

        // returns the slice count of the FabricCore::DGNode this DGPort is connected to
        unsigned int getSliceCount();

        // sets the slice count of the FabricCore::DGNode this DGPort is connected to
        bool setSliceCount(unsigned int count);

        // returns the value of a specific slice of this DGPort as a FabricCore::Variant
        FabricCore::Variant getVariant(unsigned int slice = 0);

        // sets the value of a specific slice of this DGPort from a FabricCore::Variant
        bool setVariant(FabricCore::Variant value, unsigned int slice = 0);

        // returns the value of a specific slice of this DGPort as a JSON string
        std::string getJSON(unsigned int slice = 0);

        // sets the value of a specific slice of this DGPort from a JSON string
        bool setJSON(const char * json, unsigned int slice = 0);

        // returns the default value of this DGPort as a FabricCore::Variant
        FabricCore::Variant getDefault();
    
        // returns the value of a specific slice of this DGPort as a FabricCore::RTVal
        FabricCore::RTVal getRTVal(bool evaluate = false, uint32_t slice = 0);

        // sets the value of a specific slice of this DGPort from a FabricCore::RTVal
        bool setRTVal(FabricCore::RTVal value, uint32_t slice = 0);

        // returns the size of an array member this DGPort is connected to
        unsigned int getArrayCount(unsigned int slice = 0);

        // returns the void* array data of this DGPort.
        // this only works for array DGPorts (isArray() == true)
        // the bufferSize has to match getArrayCount() * getDataSize()
        bool getArrayData(void * buffer, unsigned int bufferSize, unsigned int slice = 0);

        // sets the void* array data of this DGPort.
        // this only works for array DGPorts (isArray() == true)
        // this also sets the array count determined by bufferSize / getDataSize()
        bool setArrayData(void * buffer, unsigned int bufferSize, unsigned int slice = 0);

        // gets the void* slice array data of this DGPort.
        // this only works for non-array DGPorts (isArray() == false)
        // the bufferSize has to match getSliceCount() * getDataSize()
        bool getAllSlicesData(void * buffer, unsigned int bufferSize);

        // sets the void* slice array data of this DGPort.
        // this only works for non-array DGPorts (isArray() == false)
        // the bufferSize has to match getSliceCount() * getDataSize()
        bool setAllSlicesData(void * buffer, unsigned int bufferSize);

        // set the array data based on another port
        // this performs data replication, and only works on shallow array data ports.
        // the data type has to match as well (so only Vec3 to Vec3 for example).
        bool copyArrayDataFromDGPort(DGPort other, unsigned int slice = 0, unsigned int otherSlice = UINT_MAX);

        // set the slices data based on another port
        // this performs data replication, and only works on shallow non array data ports.
        // the data type has to match as well (so only Vec3 to Vec3 for example).
        bool copyAllSlicesDataFromDGPort(DGPort other, bool resizeTarget = false);

        // sets an auxiliary option
        void setOption(const char * name, const FabricCore::Variant & value);

        // gets an auxiliary option (empty variant if not defined)
        FabricCore::Variant getOption(const char * name);

        // returns true if an auxiliary option exists
        bool hasOption(const char * name);

        // gets an auxiliary option as boolean
        bool getBooleanOption(const char * name, bool defaultValue = false);

        // gets an auxiliary option as int
        int getIntegerOption(const char * name, int defaultValue = -1);

        // gets an auxiliary option as float
        float getScalarOption(const char * name, float defaultValue = 0.0);

        // gets an auxiliary option as string
        std::string getStringOption(const char * name, const char * defaultValue = "");

        // returns true if this port can be manipulated
        bool isManipulatable();

        // returns all animation channels of this port
        FabricCore::RTVal getAnimationChannels();

        // sets the values of all animation channels
        void setAnimationChannelValues(unsigned int nbChannels, float * values);

        // perform a manipulation and returns the event
        // int codes from ManipulationContext.kl
        int manipulate(FabricCore::RTVal & manipulationContext);
      };
    };
*/
/*SPHINX:dggraph

.. _dggraph:

FabricSplice::DGGraph
=========================

The DGGraph is the dependency graph container in the SPLICECAPI. It hosts KL operators, FabricCore elements such as dependency graph nodes, and provides access to :ref:`dgport` objects. The DGGraph class wraps the functionality of the lower level FabricCore CAPI and provides persistence functionality to store/restore the computation setup to/from files or strings. The class outline can be found further down on this page.

Example
---------------------------------

The sample below shows how to use graphs. All of the other examples also use graphs, of course. This example focuses on the persistence capability of the DGGraph class.

.. code-block:: c++

    #include <FabricSplice.h>

    using namespace FabricSplice;

    int main( int argc, const char* argv[] )
    {
      Initialize();

      // create a graph to store the nodes
      DGGraph graph = DGGraph("myGraph");

      // create two DG nodes
      // the first DG node is established
      // as the 'default' DGNode
      graph.constructDGNode("compute");
      graph.constructDGNode("data");

      // create a dependency
      graph.setDGNodeDependency("compute", "data");

      // create the members to connect
      graph.addDGNodeMember("someValues", "Scalar[]", FabricCore::Variant(), "data");
      graph.addDGNodeMember("sum", "Scalar"); // default DGNode
      graph.addDGNodeMember("product", "Scalar"); // default DGNode

      // create ports
      DGPort someValues = graph.addDGPort("someValues", "someValues", Port_Mode_IN, "data");
      DGPort sum = graph.addDGPort("sum", "sum", Port_Mode_OUT); // default DGNode
      DGPort product = graph.addDGPort("product", "product", Port_Mode_OUT); // default DGNode

      // create an operator
      std::string klCode = "";
      klCode += "operator addOp(Scalar someValues[], io Scalar sum) {";
      klCode += "  sum = 0.0;";
      klCode += "  for(Size i=0;i<someValues.size();i++)";
      klCode += "    sum += someValues[i];";
      klCode += "}";
      graph.constructKLOperator("addOp", klCode.c_str()); // default DGNode

      // create another operator
      klCode = "";
      klCode += "operator mulOp(Scalar someValues[], io Scalar product) {";
      klCode += "  product = 1.0;";
      klCode += "  for(Size i=0;i<someValues.size();i++)";
      klCode += "    product *= someValues[i];";
      klCode += "}";
      graph.constructKLOperator("mulOp", klCode.c_str()); // default DGNode

      // persist the data to json
      // (alternatively, you can also call the saveToFile method to persist to a file)
      std::string jsonData = graph.getPersistenceDataJSON();

      // destroy the ports, the node and close the FabricCore::Client
      graph = DGGraph();

      // print the persisted data
      printf("%s\n", jsonData.c_str());

      // create a new graph
      graph = DGGraph("anotherGraph");

      // reconstruct the members, ports and operators!
      // (alternatively, you can also call the loadFromFile method to persist from a file)
      graph.setFromPersistenceDataJSON(jsonData.c_str());

      // print all of the port names
      for(unsigned int i=0;i<graph.getDGPortCount();i++)
        printf("port reconstructed: %s\n", graph.getDGPortName(i).getStringData());

      // print all of the operator names
      for(unsigned int i=0;i<graph.getKLOperatorCount();i++)
        printf("operator reconstructed: %s\n", graph.getKLOperatorName(i).getStringData());

      Finalize();
      return 0;
    }


Class Outline
---------------------------------

.. code-block:: c++

    namespace FabricSplice
    {
      struct PersistenceInfo
      {
        FabricCore::Variant hostAppName;
        FabricCore::Variant hostAppVersion;
      };
      
      class DGGraph
      {
      public:
        
        // obsolete, empty constructor
        DGGraph();

        // constructor which creates a FabricCore::Client and initiates an internal FabricCore::DGNode
        DGGraph(const char * name, int guarded = 1, FabricCore::ClientOptimizationType optType = FabricCore::ClientOptimizationType_Background);

        // copy constructor
        DGGraph(DGGraph const & other);

        // copy operator
        DGGraph & operator =( DGGraph const & other );

        // returns true if the object is valid
        bool isValid() const;

        // bool conversion operator
        operator bool() const;

        // empties the content of the node
        void clear();

        // returns the FabricCore client
        static const FabricCore::Client * getClient();

        // loads the given extension
        static void loadExtension(const char *extensionName);

        // retrieve the user pointer
        void * getUserPointer();

        // store a user pointer
        void setUserPointer(void * data);

        // returns the name of this graph
        const char * getName();

        // sets the name and ensures name uniqueness
        bool setName(const char * name);

        // retrieves the metadata from the DGGraph
        const char * getMetaData();

        // sets metadata on the DGGraph
        void setMetaData(const char * json);

        // returns the number of DGNodes on this graph
        unsigned int getDGNodeCount();
    
        // returns the name of a specific DGNode in this graph
        char const * getDGNodeName(unsigned int index = 0);

        // adds a member based on a member name and type (rt)
        bool addDGNodeMember(const char * name, const char * rt, FabricCore::Variant defaultValue = FabricCore::Variant(), const char * dgNodeName = "", const char * extension = "");
        
        // returns true if a specific member exists
        bool hasDGNodeMember(const char * name, const char * dgNodeName = "");

        // removes a member
        bool removeDGNodeMember(const char * name, const char * dgNodeName = "");

        // returns KL source code for the parameter list for all available ports
        FabricCore::Variant generateKLOperatorParameterList();

        // returns dummy KL source code for a new operator
        FabricCore::Variant generateKLOperatorSourceCode(const char * name, const char * additionalBody = "", const char * additionalFunctions = "", const char * executeParallelMember = "");

        // constructs a FabricCore::DGOperator based on a name and a kl source string.
        // the portMap is a variant dict providing operator parameter name to port name mappings.
        bool constructKLOperator(const char * name, const char * sourceCode = "", const char * entry = "", const char * dgNodeName = "", const FabricCore::Variant & portMap = FabricCore::Variant::CreateDict());

        // removes a KL operator from this Node
        bool removeKLOperator(const char * name, const char * dgNodeName = "");

        // returns true if this graph contains a given KL operator
        bool hasKLOperator(const char * name, const char * dgNodeName = "");

        // gets the entry point of a specific FabricCore::DGOperator
        char const * getKLOperatorEntry(const char * name);

        // sets the entry point of a specific FabricCore::DGOperator
        bool setKLOperatorEntry(const char * name, const char * entry);

        // moves the FabricCore::DGOperator on the stack to a given index
        bool setKLOperatorIndex(const char * name, unsigned int index);

        // gets the source code of a specific FabricCore::DGOperator
        char const * getKLOperatorSourceCode(const char * name);

        // sets the source code of a specific FabricCore::DGOperator
        bool setKLOperatorSourceCode(const char * name, const char * sourceCode, const char * entry = "");

        // loads the source code of a specific FabricCore::DGOperator from file
        void loadKLOperatorSourceCode(const char * name, const char * filePath);

        // saves the source code of a specific FabricCore::DGOperator to file
        void saveKLOperatorSourceCode(const char * name, const char * filePath);

        // returns true if the KL operator is using a file
        bool isKLOperatorFileBased(const char * name);

        // gets the filepath of a specific FabricCore::DGOperator
        char const * getKLOperatorFilePath(const char * name);

        // loads the content of the file and sets the code
        void setKLOperatorFilePath(const char * name, const char * filePath, const char * entry = "");

        // returns the number of operators in this graph
        unsigned int getKLOperatorCount(const char * dgNodeName = "");

        // returns the name of a specific operator in this graph
        char const * getKLOperatorName(unsigned int index = 0, const char * dgNodeName = "");

        // returns the number of operators in total
        static unsigned int getGlobalKLOperatorCount();

        // returns the name of a specific operator
        static char const * getGlobalKLOperatorName(unsigned int index = 0);

        // checks all FabricCore::DGNodes and FabricCore::Operators for errors, return false if any errors found
        static bool checkErrors();

        // evaluates the contained DGNode
        bool evaluate();

        // clears the evaluate state
        bool clearEvaluate();

        // requires the evaluate to take place
        bool requireEvaluate();

        // returns if this graph is using the eval context
        bool usesEvalContext();

        // returns the graph's evaluation context
        FabricCore::RTVal getEvalContext();

        // adds a new Port provided a name, the member and a mode
        DGPort addDGPort(const char * name, const char * member, FabricSplice::Port_Mode mode, const char * dgNodeName = "", bool autoInitObjects = true);

        // removes an existing Port by name
        bool removeDGPort(const char * name);

        // returns a specific Port by name
        DGPort getDGPort(const char * name);

        // returns a specific Port by index
        DGPort getDGPort(unsigned int index);

        // returns the number of ports in this graph
        unsigned int getDGPortCount();

        // returns the name of a specific port in this graph
        char const * getDGPortName(unsigned int index);

        // returns JSON string encoding the port layout of the node
        std::string getDGPortInfo();

        // returns true if a DGNode of the given name exists
        bool hasDGNode(const char * dgNodeName);

        // returns the internal FabricCore::DGNode based on an index
        FabricCore::DGNode getDGNode(unsigned int index = 0);

        // returns the internal FabricCore::DGNode based on a name
        FabricCore::DGNode getDGNode(const char * dgNodeName = "");

        // creates a new DG node
        void constructDGNode(const char * dgNodeName = "");

        // removes an existing DG node
        void removeDGNode(const char * dgNodeName);

        // returns true if a given DG node is dependent on another one
        bool hasDGNodeDependency(const char * dgNode, const char * dependency);

        // depends one DGNode on another one
        bool setDGNodeDependency(const char * dgNode, const char * dependency);

        // removes the dependency of one DGNode on another one
        bool removeDGNodeDependency(const char * dgNode, const char * dependency);

        // returns variant dict of the persistence data of a node
        FabricCore::Variant getPersistenceDataDict(const PersistenceInfo * info = NULL);

        // returns JSON string encoding of the persistence data of a node
        std::string getPersistenceDataJSON(const PersistenceInfo * info = NULL);

        // constructs the node based on a variant dict
        bool setFromPersistenceDataDict(const FabricCore::Variant & dict, PersistenceInfo * info = NULL, const char * baseFilePath = NULL);

        // constructs the node based on a JSON string
        bool setFromPersistenceDataJSON(const char * json, PersistenceInfo * info = NULL, const char * baseFilePath = NULL);

        // persists the node description into a JSON file
        bool saveToFile(const char * filePath, const PersistenceInfo * info = NULL);

        // constructs the node based on a persisted JSON file
        bool loadFromFile(const char * filePath, PersistenceInfo * info = NULL, bool asReferenced = false);

        // reloads an already referenced graph from file
        bool reloadFromFile(PersistenceInfo * info = NULL);

        // returns true if this graph is referenced from a file
        bool isReferenced();

        // returns the splice file path referenced by this graph
        const char * getReferencedFilePath();

        // marks a member to be persisted
        void setMemberPersistence(const char * name, bool persistence);
      };
    };
*/

#include <stdint.h>
#include <stdio.h>

#if defined(_MSC_VER) || defined(SWIGWIN)
# if defined(__cplusplus)
#  define FECS_IMPORT_SHARED extern "C" __declspec(dllimport)
#  define FECS_EXPORT_SHARED extern "C" __declspec(dllexport)
# else
#  define FECS_IMPORT_SHARED __declspec(dllimport)
#  define FECS_EXPORT_SHARED __declspec(dllexport)
# endif
#else
# if defined(SWIG)
#  define FECS_IMPORT_SHARED extern "C"
#  define FECS_EXPORT_SHARED extern "C"
# elif defined(__cplusplus)
#  define FECS_IMPORT_SHARED extern "C" __attribute__ ((visibility("default")))
#  define FECS_EXPORT_SHARED extern "C" __attribute__ ((visibility("default")))
# else
#  define FECS_IMPORT_SHARED __attribute__ ((visibility("default")))
#  define FECS_EXPORT_SHARED __attribute__ ((visibility("default")))
# endif
#endif
 
#if defined(__cplusplus)
# define FECS_IMPORT_STATIC extern "C"
# define FECS_EXPORT_STATIC extern "C"
#else
# define FECS_IMPORT_STATIC
# define FECS_EXPORT_STATIC
#endif

#if defined(FECS_SHARED)
# if defined(FECS_BUILDING)
#  define FECS_DECL FECS_EXPORT_SHARED
# else
#  define FECS_DECL FECS_IMPORT_SHARED
# endif
#elif defined(FECS_STATIC) 
# if defined(FECS_BUILDING)
#  define FECS_DECL FECS_EXPORT_STATIC
# else
#  define FECS_DECL FECS_IMPORT_STATIC
# endif
#else
# error "Must define one of: FECS_STATIC, FECS_SHARED"
#endif

#include <limits.h>
#include <stdlib.h>
#include <FabricCore.h>

// C typedefs
//=====================================================

//
// This template class allows for defining explicit boolean
// version operators without C++0x
//
// This code is taken (mostly) verbatim from 
// http://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Safe_bool
// modified so the actual conversion operators is no hidden on the base class 
//
// Usage is like so
//
//
//  class YourClass : 
//    public safe_bool <YourClass> // CRTP idiom
//  {
//  public:
//    // Define boolean conversion
//    operator explicit_bool::type() const {
//      return explicit_bool::get(SomeLogicHere()); 
//    }
//  };
//
//   This will allow
// if (yourClass)
//   but not
// int var = yourClass;
class explicit_bool {
  public:
    typedef void (explicit_bool::*type)() const;
    void this_type_does_not_support_comparisons() const {}
  
    static type get(bool condition) {
        return condition ? &explicit_bool::this_type_does_not_support_comparisons : 0;
    }
};

struct FECS_PersistenceInfo
{
  FabricCore::Variant hostAppName;
  FabricCore::Variant hostAppVersion;
  FabricCore::Variant filePath;
};

typedef void * FECS_DGGraphRef;
typedef void * FECS_DGPortRef;
typedef void * FECS_KLParserRef;
typedef void * FECS_KLParserSymbolRef;
typedef void * FECS_KLParserConstantRef;
typedef void * FECS_KLParserVariableRef;
typedef void * FECS_KLParserInterfaceRef;
typedef void * FECS_KLParserStructRef;
typedef void * FECS_KLParserArgumentListRef;
typedef void * FECS_KLParserOperatorRef;
typedef void * FECS_KLParserFunctionRef;
typedef void(*FECS_LoggingFunc)(const char * message, unsigned int messageLength);
typedef void(*FECS_CompilerErrorFunc)(unsigned int row, unsigned int col, const char * file, const char * level, const char * desc);
typedef void(*FECS_StatusFunc)(const char * topic, unsigned int topicLength, const char * message, unsigned int messageLength);
typedef void(*FECS_SlowOperationFunc)(const char *descCStr, unsigned int descLength );
typedef const char *(*FECS_GetOperatorSourceCodeFunc)(const char * graphName, const char * opName);

enum FECS_DGPort_Mode
{
  FECS_DGPort_Mode_IN = 0,
  FECS_DGPort_Mode_OUT = 1,
  FECS_DGPort_Mode_IO = 2
};

typedef FEC_LockType FECS_LockType;
#define FECS_LockType_Shared FEC_LockType_Shared
#define FECS_LockType_Exclusive FEC_LockType_Exclusive

// C functions
//=====================================================
FECS_DECL void FECS_Initialize();
FECS_DECL void FECS_Finalize();
FECS_DECL uint8_t FECS_GetFabricVersionMaj();
FECS_DECL uint8_t FECS_GetFabricVersionMin();
FECS_DECL uint8_t FECS_GetFabricVersionRev();
FECS_DECL const char * FECS_GetFabricVersionStr();
FECS_DECL const char * FECS_GetSpliceVersion();
FECS_DECL void FECS_constructClient(FabricCore::Client & client, int guarded, FabricCore::ClientOptimizationType optType);
FECS_DECL void FECS_setLicenseType(FabricCore::ClientLicenseType licenseType);
FECS_DECL bool FECS_destroyClient(bool force);
FECS_DECL char const * FECS_GetClientContextID();
FECS_DECL bool FECS_addExtFolder(const char * folder);
FECS_DECL void FECS_setDCCOperatorSourceCodeCallback(FECS_GetOperatorSourceCodeFunc func);
FECS_DECL void FECS_ConstructRTVal(FabricCore::RTVal & result, const char * rt);
FECS_DECL void FECS_ConstructRTValArgs(FabricCore::RTVal & result, const char * rt, uint32_t nbArgs, const FabricCore::RTVal * args);
FECS_DECL void FECS_ConstructObjectRTVal(FabricCore::RTVal & result, const char * rt);
FECS_DECL void FECS_ConstructObjectRTValArgs(FabricCore::RTVal & result, const char * rt, uint32_t nbArgs, const FabricCore::RTVal * args);
FECS_DECL void FECS_ConstructInterfaceRTValArgs(FabricCore::RTVal & result, const char * rt, const FabricCore::RTVal & object);
FECS_DECL void FECS_ConstructBooleanRTVal(FabricCore::RTVal & result, bool value);
FECS_DECL void FECS_ConstructSInt8RTVal(FabricCore::RTVal & result, int8_t value);
FECS_DECL void FECS_ConstructSInt16RTVal(FabricCore::RTVal & result, int16_t value);
FECS_DECL void FECS_ConstructSInt32RTVal(FabricCore::RTVal & result, int32_t value);
FECS_DECL void FECS_ConstructSInt64RTVal(FabricCore::RTVal & result, int64_t value);
FECS_DECL void FECS_ConstructUInt8RTVal(FabricCore::RTVal & result, uint8_t value);
FECS_DECL void FECS_ConstructUInt16RTVal(FabricCore::RTVal & result, uint16_t value);
FECS_DECL void FECS_ConstructUInt32RTVal(FabricCore::RTVal & result, uint32_t value);
FECS_DECL void FECS_ConstructUInt64RTVal(FabricCore::RTVal & result, uint64_t value);
FECS_DECL void FECS_ConstructFloat32RTVal(FabricCore::RTVal & result, float value);
FECS_DECL void FECS_ConstructFloat64RTVal(FabricCore::RTVal & result, double value);
FECS_DECL void FECS_ConstructDataRTVal(FabricCore::RTVal & result, void *value);
FECS_DECL void FECS_ConstructStringRTVal(FabricCore::RTVal & result, const char * value);
FECS_DECL void FECS_ConstructVariableArrayRTVal(FabricCore::RTVal & result, const char * rt);
FECS_DECL void FECS_ConstructExternalArrayRTVal(FabricCore::RTVal & result, const char * rt, uint32_t nbElements, void * data);

FECS_DECL const char * FECS_KLParser_owner(FECS_KLParserRef ref);
FECS_DECL const char * FECS_KLParser_name(FECS_KLParserRef ref);
FECS_DECL const char * FECS_KLParser_code(FECS_KLParserRef ref);
FECS_DECL unsigned int FECS_KLParser_getNbParsers();
FECS_DECL FECS_KLParserRef FECS_KLParser_getParserFromIndex(unsigned int index);
FECS_DECL FECS_KLParserRef FECS_KLParser_getParser(const char * owner, const char * name, const char * klCode);
FECS_DECL FECS_KLParserRef FECS_KLParser_copy(FECS_KLParserRef ref);
FECS_DECL void FECS_KLParser_destroy(FECS_KLParserRef ref);
FECS_DECL bool FECS_KLParser_parse(FECS_KLParserRef ref, const char * klCode);
FECS_DECL unsigned int FECS_KLParser_getNbKLSymbols(FECS_KLParserRef ref);
FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_getKLSymbol(FECS_KLParserRef ref, unsigned int symbolIndex);
FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_getKLSymbolFromCharIndex(FECS_KLParserRef ref, unsigned int charIndex);
FECS_DECL unsigned int FECS_KLParser_getNbKLRequires(FECS_KLParserRef ref);
FECS_DECL const char * FECS_KLParser_getKLRequire(FECS_KLParserRef ref, unsigned int index);
FECS_DECL unsigned int FECS_KLParser_getNbKLConstants(FECS_KLParserRef ref);
FECS_DECL FECS_KLParserConstantRef FECS_KLParser_getKLConstant(FECS_KLParserRef ref, unsigned int index);
FECS_DECL unsigned int FECS_KLParser_getNbKLVariables(FECS_KLParserRef ref);
FECS_DECL FECS_KLParserVariableRef FECS_KLParser_getKLVariable(FECS_KLParserRef ref, unsigned int index);
FECS_DECL unsigned int FECS_KLParser_getNbKLInterfaces(FECS_KLParserRef ref);
FECS_DECL FECS_KLParserInterfaceRef FECS_KLParser_getKLInterface(FECS_KLParserRef ref, unsigned int index);
FECS_DECL unsigned int FECS_KLParser_getNbKLStructs(FECS_KLParserRef ref);
FECS_DECL FECS_KLParserStructRef FECS_KLParser_getKLStruct(FECS_KLParserRef ref, unsigned int index);
FECS_DECL unsigned int FECS_KLParser_getNbKLOperators(FECS_KLParserRef ref);
FECS_DECL FECS_KLParserOperatorRef FECS_KLParser_getKLOperator(FECS_KLParserRef ref, unsigned int index);
FECS_DECL unsigned int FECS_KLParser_getNbKLFunctions(FECS_KLParserRef ref);
FECS_DECL FECS_KLParserFunctionRef FECS_KLParser_getKLFunction(FECS_KLParserRef ref, unsigned int index);
FECS_DECL const char * FECS_KLParser_getKLTypeForSymbol(FECS_KLParserRef ref, FECS_KLParserSymbolRef symbol);
FECS_DECL const char * FECS_KLParser_getKLTypeForMemberOrMethod(const char * owner, const char * member);

FECS_DECL unsigned int FECS_KLParser_KLSymbol_index(FECS_KLParserSymbolRef ref);
FECS_DECL unsigned int FECS_KLParser_KLSymbol_pos(FECS_KLParserSymbolRef ref);
FECS_DECL unsigned int FECS_KLParser_KLSymbol_length(FECS_KLParserSymbolRef ref);
FECS_DECL int FECS_KLParser_KLSymbol_type(FECS_KLParserSymbolRef ref);
FECS_DECL bool FECS_KLParser_KLSymbol_isKeyword(FECS_KLParserSymbolRef ref);
FECS_DECL const char * FECS_KLParser_KLSymbol_typeName(FECS_KLParserSymbolRef ref);
FECS_DECL char FECS_KLParser_KLSymbol_front(FECS_KLParserSymbolRef ref, unsigned int index);
FECS_DECL char FECS_KLParser_KLSymbol_back(FECS_KLParserSymbolRef ref, unsigned int index);
FECS_DECL char * FECS_KLParser_KLSymbol_str(FECS_KLParserSymbolRef ref);
FECS_DECL bool FECS_KLParser_KLSymbol_contains(FECS_KLParserSymbolRef ref, char c);
FECS_DECL unsigned int FECS_KLParser_KLSymbol_find(FECS_KLParserSymbolRef ref, char c);
FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_KLSymbol_prev(FECS_KLParserSymbolRef ref, bool skipComments, unsigned int offset);
FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_KLSymbol_next(FECS_KLParserSymbolRef ref, bool skipComments, unsigned int offset);
FECS_DECL const char * FECS_KLParser_KLSymbol_parser(FECS_KLParserSymbolRef ref);
FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_KLConstant_symbol(FECS_KLParserConstantRef ref);
FECS_DECL const char * FECS_KLParser_KLConstant_comments(FECS_KLParserConstantRef ref);
FECS_DECL const char * FECS_KLParser_KLConstant_type(FECS_KLParserConstantRef ref);
FECS_DECL const char * FECS_KLParser_KLConstant_name(FECS_KLParserConstantRef ref);
FECS_DECL const char * FECS_KLParser_KLConstant_value(FECS_KLParserConstantRef ref);
FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_KLVariable_symbol(FECS_KLParserVariableRef ref);
FECS_DECL const char * FECS_KLParser_KLVariable_type(FECS_KLParserVariableRef ref);
FECS_DECL const char * FECS_KLParser_KLVariable_name(FECS_KLParserVariableRef ref);
FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_KLStruct_symbol(FECS_KLParserStructRef ref);
FECS_DECL const char * FECS_KLParser_KLStruct_comments(FECS_KLParserStructRef ref);
FECS_DECL const char * FECS_KLParser_KLStruct_type(FECS_KLParserStructRef ref);
FECS_DECL const char * FECS_KLParser_KLStruct_name(FECS_KLParserStructRef ref);
FECS_DECL unsigned int FECS_KLParser_KLStruct_nbInterfaces(FECS_KLParserStructRef ref);
FECS_DECL const char * FECS_KLParser_KLStruct_interface(FECS_KLParserStructRef ref, unsigned int index);
FECS_DECL unsigned int FECS_KLParser_KLStruct_nbMembers(FECS_KLParserStructRef ref);
FECS_DECL const char * FECS_KLParser_KLStruct_memberType(FECS_KLParserStructRef ref, unsigned int index);
FECS_DECL const char * FECS_KLParser_KLStruct_memberName(FECS_KLParserStructRef ref, unsigned int index);
FECS_DECL unsigned int FECS_KLParser_KLArgumentList_nbArgs(FECS_KLParserArgumentListRef ref);
FECS_DECL const char * FECS_KLParser_KLArgumentList_mode(FECS_KLParserArgumentListRef ref, unsigned int index);
FECS_DECL const char * FECS_KLParser_KLArgumentList_type(FECS_KLParserArgumentListRef ref, unsigned int index);
FECS_DECL const char * FECS_KLParser_KLArgumentList_name(FECS_KLParserArgumentListRef ref, unsigned int index);
FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_KLOperator_symbol(FECS_KLParserOperatorRef ref);
FECS_DECL const char * FECS_KLParser_KLOperator_comments(FECS_KLParserOperatorRef ref);
FECS_DECL const char * FECS_KLParser_KLOperator_name(FECS_KLParserOperatorRef ref);
FECS_DECL bool FECS_KLParser_KLOperator_isPex(FECS_KLParserOperatorRef ref);
FECS_DECL const char * FECS_KLParser_KLOperator_pexArgument(FECS_KLParserOperatorRef ref);
FECS_DECL FECS_KLParserArgumentListRef FECS_KLParser_KLOperator_arguments(FECS_KLParserOperatorRef ref);
FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_KLOperator_bodyStart(FECS_KLParserOperatorRef ref);
FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_KLOperator_bodyEnd(FECS_KLParserOperatorRef ref);
FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_KLFunction_symbol(FECS_KLParserFunctionRef ref);
FECS_DECL const char * FECS_KLParser_KLFunction_comments(FECS_KLParserFunctionRef ref);
FECS_DECL const char * FECS_KLParser_KLFunction_type(FECS_KLParserFunctionRef ref);
FECS_DECL const char * FECS_KLParser_KLFunction_owner(FECS_KLParserFunctionRef ref);
FECS_DECL const char * FECS_KLParser_KLFunction_name(FECS_KLParserFunctionRef ref);
FECS_DECL FECS_KLParserArgumentListRef FECS_KLParser_KLFunction_arguments(FECS_KLParserFunctionRef ref);
FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_KLFunction_bodyStart(FECS_KLParserFunctionRef ref);
FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_KLFunction_bodyEnd(FECS_KLParserFunctionRef ref);
FECS_DECL FECS_KLParserSymbolRef FECS_KLParser_KLInterface_symbol(FECS_KLParserInterfaceRef ref);
FECS_DECL const char * FECS_KLParser_KLInterface_comments(FECS_KLParserInterfaceRef ref);
FECS_DECL const char * FECS_KLParser_KLInterface_name(FECS_KLParserInterfaceRef ref);
FECS_DECL unsigned int FECS_KLParser_KLInterface_nbFunctions(FECS_KLParserInterfaceRef ref);
FECS_DECL FECS_KLParserFunctionRef FECS_KLParser_KLInterface_function(FECS_KLParserInterfaceRef ref, unsigned int index);
FECS_DECL void FECS_Logging_setLogFunc(FECS_LoggingFunc func);
FECS_DECL void FECS_Logging_setLogErrorFunc(FECS_LoggingFunc func);
FECS_DECL void FECS_Logging_setCompilerErrorFunc(FECS_CompilerErrorFunc func);
FECS_DECL void FECS_Logging_setKLReportFunc(FECS_LoggingFunc func);
FECS_DECL void FECS_Logging_setKLStatusFunc(FECS_StatusFunc func);
FECS_DECL void FECS_Logging_setSlowOperationFunc(FECS_SlowOperationFunc func);
FECS_DECL bool FECS_Logging_hasError();
FECS_DECL char const * FECS_Logging_getError();
FECS_DECL void FECS_Logging_clearError();
FECS_DECL void FECS_Logging_enableTimers(); 
FECS_DECL void FECS_Logging_disableTimers(); 
FECS_DECL void FECS_Logging_resetTimer(const char * name);
FECS_DECL void FECS_Logging_startTimer(const char * name);
FECS_DECL void FECS_Logging_stopTimer(const char * name);
FECS_DECL void FECS_Logging_logTimer(const char * name);
FECS_DECL unsigned int FECS_Logging_getNbTimers();
FECS_DECL char const * FECS_Logging_getTimerName(unsigned int index);

// FECS_DECL void FECS_SceneManagement_setManipulationFunc(FECS_ManipulationFunc func);
FECS_DECL bool FECS_SceneManagement_hasRenderableContent();
FECS_DECL void FECS_SceneManagement_drawOpenGL(FabricCore::RTVal & drawContext);
// FECS_DECL bool FECS_SceneManagement_raycast(FabricCore::RTVal & raycastContext, FECS_DGPortRef & portRef);

FECS_DECL void FECS_Scripting_parseScriptingArguments(const char * action, const char * reference, const char * data, const char * auxiliary, FabricCore::Variant & result);
FECS_DECL bool FECS_Scripting_consumeBooleanArgument(FabricCore::Variant & argsDict, const char * name, bool defaultValue, bool optional);
FECS_DECL int FECS_Scripting_consumeIntegerArgument(FabricCore::Variant & argsDict, const char * name, int defaultValue, bool optional);
FECS_DECL float FECS_Scripting_consumeScalarArgument(FabricCore::Variant & argsDict, const char * name, float defaultValue, bool optional);
FECS_DECL char * FECS_Scripting_consumeStringArgument(FabricCore::Variant & argsDict, const char * name, const char * defaultValue, bool optional);
FECS_DECL void FECS_Scripting_consumeVariantArgument(FabricCore::Variant & argsDict, const char * name, const FabricCore::Variant & defaultValue, bool optional, FabricCore::Variant & result);

FECS_DECL FECS_DGGraphRef FECS_DGGraph_construct(const char * name, int guarded, FabricCore::ClientOptimizationType optType);
FECS_DECL FECS_DGGraphRef FECS_DGGraph_copy(FECS_DGGraphRef ref);
FECS_DECL void FECS_DGGraph_destroy(FECS_DGGraphRef ref);
FECS_DECL void FECS_DGGraph_clear(FECS_DGGraphRef ref);
FECS_DECL void FECS_DGGraph_getClient(const FabricCore::Client ** client);
FECS_DECL void FECS_DGGraph_loadExtension(const char *extensionName);
FECS_DECL void * FECS_DGGraph_getUserPointer(FECS_DGGraphRef ref);
FECS_DECL void FECS_DGGraph_setUserPointer(FECS_DGGraphRef ref, void * data);

FECS_DECL char const * FECS_DGGraph_getName(FECS_DGGraphRef ref);
FECS_DECL bool FECS_DGGraph_setName(FECS_DGGraphRef ref, const char * name);
FECS_DECL char const * FECS_DGGraph_getMetaData(FECS_DGGraphRef ref);
FECS_DECL void FECS_DGGraph_setMetaData(FECS_DGGraphRef ref, const char * json);
FECS_DECL unsigned int FECS_DGGraph_getDGNodeCount(FECS_DGGraphRef ref);
FECS_DECL char const * FECS_DGGraph_getDGNodeName(FECS_DGGraphRef ref, unsigned int index);
FECS_DECL bool FECS_DGGraph_hasDGNode(FECS_DGGraphRef ref, const char * dgNodeName);
FECS_DECL void FECS_DGGraph_getDGNode(FECS_DGGraphRef ref, FabricCore::DGNode & dgNode, const char * dgNodeName);
FECS_DECL void FECS_DGGraph_getDGNodeByIndex(FECS_DGGraphRef ref, FabricCore::DGNode & dgNode, unsigned int index);
FECS_DECL void FECS_DGGraph_constructDGNode(FECS_DGGraphRef ref, const char * dgNodeName);
FECS_DECL void FECS_DGGraph_removeDGNode(FECS_DGGraphRef ref, const char * dgNodeName);
FECS_DECL bool FECS_DGGraph_addDGNodeMember(FECS_DGGraphRef ref, const char * name, const char * rt, FabricCore::Variant defaultValue, const char * dgNodeName, const char * extension);
FECS_DECL bool FECS_DGGraph_hasDGNodeMember(FECS_DGGraphRef ref, const char * name, const char * dgNodeName);
FECS_DECL bool FECS_DGGraph_removeDGNodeMember(FECS_DGGraphRef ref, const char * name, const char * dgNodeName);
FECS_DECL void FECS_DGGraph_generateKLOperatorParameterList(FECS_DGGraphRef ref, FabricCore::Variant & code);
FECS_DECL void FECS_DGGraph_generateKLOperatorSourceCode(FECS_DGGraphRef ref, FabricCore::Variant & code, const char * name, const char * additionalBody, const char * additionalFunctions, const char * executeParallelMember);
FECS_DECL bool FECS_DGGraph_constructKLOperator(FECS_DGGraphRef ref, const char * name, const char * sourceCode, const char * entry, const char * dgNodeName, const FabricCore::Variant & portMap);
FECS_DECL bool FECS_DGGraph_removeKLOperator(FECS_DGGraphRef ref, const char * name, const char * dgNodeName);
FECS_DECL bool FECS_DGGraph_hasKLOperator(FECS_DGGraphRef ref, const char * name, const char * dgNodeName);
FECS_DECL char const * FECS_DGGraph_getKLOperatorEntry(FECS_DGGraphRef ref, const char * name);
FECS_DECL bool FECS_DGGraph_setKLOperatorEntry(FECS_DGGraphRef ref, const char * name, const char * entry);
FECS_DECL bool FECS_DGGraph_setKLOperatorIndex(FECS_DGGraphRef ref, const char * name, unsigned int index);
FECS_DECL char const * FECS_DGGraph_getKLOperatorSourceCode(FECS_DGGraphRef ref, const char * name);
FECS_DECL bool FECS_DGGraph_setKLOperatorSourceCode(FECS_DGGraphRef ref, const char * name, const char * sourceCode, const char * entry);
FECS_DECL void FECS_DGGraph_loadKLOperatorSourceCode(FECS_DGGraphRef ref, const char * name, const char * filePath);
FECS_DECL void FECS_DGGraph_saveKLOperatorSourceCode(FECS_DGGraphRef ref, const char * name, const char * filePath);
FECS_DECL bool FECS_DGGraph_isKLOperatorFileBased(FECS_DGGraphRef ref, const char * name);
FECS_DECL char const * FECS_DGGraph_getKLOperatorFilePath(FECS_DGGraphRef ref, const char * name);
FECS_DECL void FECS_DGGraph_setKLOperatorFilePath(FECS_DGGraphRef ref, const char * name, const char * filePath, const char * entry);
FECS_DECL unsigned int FECS_DGGraph_getKLOperatorCount(FECS_DGGraphRef ref, const char * dgNodeName);
FECS_DECL char const * FECS_DGGraph_getKLOperatorName(FECS_DGGraphRef ref, unsigned int index, const char * dgNodeName);
FECS_DECL unsigned int FECS_DGGraph_getGlobalKLOperatorCount();
FECS_DECL char const * FECS_DGGraph_getGlobalKLOperatorName(unsigned int index);
FECS_DECL bool FECS_DGGraph_checkErrors();
FECS_DECL void FECS_DGGraph_setEvaluateShared(FECS_DGGraphRef ref, bool evaluateShared);
FECS_DECL bool FECS_DGGraph_evaluate(FECS_DGGraphRef ref);
FECS_DECL bool FECS_DGGraph_clearEvaluate(FECS_DGGraphRef ref);
FECS_DECL bool FECS_DGGraph_usesEvalContext(FECS_DGGraphRef ref);
FECS_DECL bool FECS_DGGraph_requireEvaluate(FECS_DGGraphRef ref);
FECS_DECL void FECS_DGGraph_getEvalContext(FECS_DGPortRef ref, FabricCore::RTVal & result);
FECS_DECL FECS_DGPortRef FECS_DGGraph_addDGPort(FECS_DGGraphRef ref, const char * name, const char * member, FECS_DGPort_Mode mode, const char * dgNodeName, bool autoInitObjects);
FECS_DECL bool FECS_DGGraph_removeDGPort(FECS_DGGraphRef ref, const char * name);
FECS_DECL FECS_DGPortRef FECS_DGGraph_getDGPort(FECS_DGGraphRef ref, const char * name);
FECS_DECL FECS_DGPortRef FECS_DGGraph_getDGPortByIndex(FECS_DGGraphRef ref, unsigned int index);
FECS_DECL unsigned int FECS_DGGraph_getDGPortCount(FECS_DGGraphRef ref);
FECS_DECL char const * FECS_DGGraph_getDGPortName(FECS_DGGraphRef ref, unsigned int index);
FECS_DECL char * FECS_DGGraph_getDGPortInfo(FECS_DGGraphRef ref);
FECS_DECL bool FECS_DGGraph_hasDGNodeDependency(FECS_DGGraphRef ref, const char * dgNode, const char * dependency);
FECS_DECL bool FECS_DGGraph_setDGNodeDependency(FECS_DGGraphRef ref, const char * dgNode, const char * dependency);
FECS_DECL bool FECS_DGGraph_removeDGNodeDependency(FECS_DGGraphRef ref, const char * dgNode, const char * dependency);
FECS_DECL void FECS_DGGraph_getPersistenceDataDict(FECS_DGGraphRef ref, FabricCore::Variant & dict, const FECS_PersistenceInfo * info);
FECS_DECL char * FECS_DGGraph_getPersistenceDataJSON(FECS_DGGraphRef ref, const FECS_PersistenceInfo * info);
FECS_DECL bool FECS_DGGraph_setFromPersistenceDataDict(FECS_DGGraphRef ref, const FabricCore::Variant & dict, FECS_PersistenceInfo * info, const char * baseFilePath);
FECS_DECL bool FECS_DGGraph_setFromPersistenceDataJSON(FECS_DGGraphRef ref, const char * json, FECS_PersistenceInfo * info, const char * baseFilePath);
FECS_DECL bool FECS_DGGraph_saveToFile(FECS_DGGraphRef ref, const char * filePath, const FECS_PersistenceInfo * info);
FECS_DECL bool FECS_DGGraph_loadFromFile(FECS_DGGraphRef ref, const char * filePath, FECS_PersistenceInfo * info, bool asReferenced);
FECS_DECL bool FECS_DGGraph_reloadFromFile(FECS_DGGraphRef ref, FECS_PersistenceInfo * info);
FECS_DECL bool FECS_DGGraph_isReferenced(FECS_DGGraphRef ref);
FECS_DECL char const * FECS_DGGraph_getReferencedFilePath(FECS_DGGraphRef ref);
FECS_DECL void FECS_DGGraph_setMemberPersistence(FECS_DGGraphRef ref, const char * name, bool persistence);

FECS_DECL FECS_DGPortRef FECS_DGPort_copy(FECS_DGPortRef ref);
FECS_DECL void FECS_DGPort_destroy(FECS_DGPortRef ref);
FECS_DECL unsigned int FECS_DGPort_getMode(FECS_DGPortRef ref);
FECS_DECL void FECS_DGPort_setMode(FECS_DGPortRef ref, unsigned int mode);
FECS_DECL char const * FECS_DGPort_getName(FECS_DGPortRef ref);
FECS_DECL char const * FECS_DGPort_getMember(FECS_DGPortRef ref);
FECS_DECL char const * FECS_DGPort_getDGNodeName(FECS_DGPortRef ref);
FECS_DECL char const * FECS_DGPort_getKey(FECS_DGPortRef ref);
FECS_DECL char const * FECS_DGPort_getDataType(FECS_DGPortRef ref);
FECS_DECL unsigned int FECS_DGPort_getDataSize(FECS_DGPortRef ref);
FECS_DECL bool FECS_DGPort_isShallow(FECS_DGPortRef ref);
FECS_DECL bool FECS_DGPort_isArray(FECS_DGPortRef ref);
FECS_DECL bool FECS_DGPort_isStruct(FECS_DGPortRef ref);
FECS_DECL bool FECS_DGPort_isObject(FECS_DGPortRef ref);
FECS_DECL bool FECS_DGPort_isInterface(FECS_DGPortRef ref);
FECS_DECL bool FECS_DGPort_doesAutoInitObjects(FECS_DGPortRef ref);
FECS_DECL unsigned int FECS_DGPort_getSliceCount(FECS_DGPortRef ref); 
FECS_DECL bool FECS_DGPort_setSliceCount(FECS_DGPortRef ref, unsigned int count); 
FECS_DECL void FECS_DGPort_getVariant(FECS_DGPortRef ref, unsigned int slice, FabricCore::Variant & result);
FECS_DECL bool FECS_DGPort_setVariant(FECS_DGPortRef ref, const FabricCore::Variant & value, unsigned int slice);
FECS_DECL char * FECS_DGPort_getJSON(FECS_DGPortRef ref, unsigned int slice);
FECS_DECL bool FECS_DGPort_setJSON(FECS_DGPortRef ref, const char * json, unsigned int slice);
FECS_DECL void FECS_DGPort_getDefault(FECS_DGPortRef ref, FabricCore::Variant & result);
FECS_DECL void FECS_DGPort_getRTVal(FECS_DGPortRef ref, bool evaluate, unsigned int slice, FabricCore::RTVal & result);
FECS_DECL bool FECS_DGPort_setRTVal(FECS_DGPortRef ref, const FabricCore::RTVal & value, unsigned int slice);
FECS_DECL unsigned int FECS_DGPort_getArrayCount(FECS_DGPortRef ref, unsigned int slice);
FECS_DECL bool FECS_DGPort_getArrayData(FECS_DGPortRef ref, void * buffer, unsigned int bufferSize, unsigned int slice);
FECS_DECL bool FECS_DGPort_setArrayData(FECS_DGPortRef ref, void * buffer, unsigned int bufferSize, unsigned int slice);
FECS_DECL bool FECS_DGPort_getAllSlicesData(FECS_DGPortRef ref, void * buffer, unsigned int bufferSize);
FECS_DECL bool FECS_DGPort_setAllSlicesData(FECS_DGPortRef ref, void * buffer, unsigned int bufferSize);
FECS_DECL bool FECS_DGPort_copyArrayDataFromPort(FECS_DGPortRef ref, FECS_DGPortRef otherRef, unsigned int slice, unsigned int otherSlice);
FECS_DECL bool FECS_DGPort_copyAllSlicesDataFromPort(FECS_DGPortRef ref, FECS_DGPortRef otherRef, bool resizeTarget);
FECS_DECL void FECS_DGPort_setOption(FECS_DGPortRef ref, const char * name, const FabricCore::Variant & value);
FECS_DECL void FECS_DGPort_getOption(FECS_DGPortRef ref, const char * name, FabricCore::Variant & result);
// FECS_DECL bool FECS_DGPort_isManipulatable(FECS_DGPortRef ref);
// FECS_DECL void FECS_DGPort_getAnimationChannels(FECS_DGPortRef ref, FabricCore::RTVal & result);
// FECS_DECL void FECS_DGPort_setAnimationChannelValues(FECS_DGPortRef ref, unsigned int nbChannels, float * values);
// FECS_DECL int FECS_DGPort_manipulate(FECS_DGPortRef ref, FabricCore::RTVal & manipulationContext);
FECS_DECL void FECS_Free(void *ptr);

#ifdef __cplusplus

#include <string>

namespace FabricSplice
{
  // a function used to receive a single message string
  typedef FECS_LoggingFunc LoggingFunc;

  // a function used to receive a compiler error
  typedef FECS_CompilerErrorFunc CompilerErrorFunc;

  // a function to receive information from a KL status message
  typedef FECS_StatusFunc StatusFunc;

  // a function to be called when slow operations start or finish
  typedef FECS_SlowOperationFunc SlowOperationFunc;

  // a data set providing all manipulation data
  // typedef FECS_ManipulationData ManipulationData;

  // // a function to be called when a manipulation has happened
  // typedef FECS_ManipulationFunc ManipulationFunc;

  // a data set providing all manipulation data
  typedef FECS_PersistenceInfo PersistenceInfo;

  // forward declarations
  class DGGraph;
  class DGPort;

  enum Port_Mode
  {
    Port_Mode_IN = FECS_DGPort_Mode_IN,
    Port_Mode_OUT = FECS_DGPort_Mode_OUT,
    Port_Mode_IO = FECS_DGPort_Mode_IO
  };

  typedef FECS_LockType LockType;
  static const LockType LockType_Shared = FEC_LockType_Shared;
  static const LockType LockType_Exclusive = FEC_LockType_Exclusive;

  class Exception
  {
  protected:
    Exception( const char * message )
    {
      mMessage = message;
    }

  private:
    const char * mMessage;
    
  public:
    char const * what() const
    {
      return mMessage;
    }
    
    operator const char *() const
    {
      return mMessage;
    }
    
    static void MaybeThrow()
    {
      if(!FECS_Logging_hasError())
        return;
      printf("%s\n", FECS_Logging_getError());
      throw Exception( FECS_Logging_getError() );
    }
  };

  inline void Initialize()
  {
    FECS_Initialize();
    Exception::MaybeThrow();
  }

  inline void Finalize()
  {
    FECS_Finalize();
    Exception::MaybeThrow();
  }

  inline uint8_t GetFabricVersionMaj()
  {
    uint8_t result = FECS_GetFabricVersionMaj();
    Exception::MaybeThrow();
    return result;
  }

  inline uint8_t GetFabricVersionMin()
  {
    uint8_t result = FECS_GetFabricVersionMin();
    Exception::MaybeThrow();
    return result;
  }

  inline uint8_t GetFabricVersionRev()
  {
    uint8_t result = FECS_GetFabricVersionRev();
    Exception::MaybeThrow();
    return result;
  }

  inline const char *GetFabricVersionStr()
  {
    const char *result = FECS_GetFabricVersionStr();
    Exception::MaybeThrow();
    return result;
  }

  inline const char * GetSpliceVersion()
  {
    const char * result = FECS_GetSpliceVersion();
    Exception::MaybeThrow();
    return result;
  }

  inline FabricCore::Client ConstructClient(int guarded = 1, FabricCore::ClientOptimizationType optType = FabricCore::ClientOptimizationType_Background)
  {
    FabricCore::Client result;
    FECS_constructClient(result, guarded, optType);
    Exception::MaybeThrow();
    return result;
  }

  inline void SetLicenseType(FabricCore::ClientLicenseType licenseType)
  {
    FECS_setLicenseType(licenseType);
    Exception::MaybeThrow();
  }

  inline bool DestroyClient(bool force = false)
  {
    bool result = FECS_destroyClient(force);
    Exception::MaybeThrow();
    return result;
  }

  inline char const * GetClientContextID()
  {
    char const * result = FECS_GetClientContextID();
    Exception::MaybeThrow();
    return result;
  }

  inline bool addExtFolder(const char * folder)
  {
    bool result = FECS_addExtFolder(folder);
    Exception::MaybeThrow();
    return result;
  }

  // a DCC callback function to gather KL operator code from the UI
  typedef FECS_GetOperatorSourceCodeFunc GetOperatorSourceCodeFunc;

  // set a callback to allow the splice persistence framework to gather
  // the last unsaved code for a given KL operator. this code might still
  // sit in the UI somewhere but hasn't been pushed to the DGGraph.
  inline void setDCCOperatorSourceCodeCallback(GetOperatorSourceCodeFunc func)
  {
    FECS_setDCCOperatorSourceCodeCallback(func);
    Exception::MaybeThrow();
  }

  // creates a RTVal just given a KL type name
  inline FabricCore::RTVal constructRTVal(const char * rt)
  {
    FabricCore::RTVal result;
    FECS_ConstructRTVal(result, rt);
    Exception::MaybeThrow();
    return result;
  }

  // creates a RTVal given a KL type name and construction args
  inline FabricCore::RTVal constructRTVal(const char * rt, uint32_t nbArgs, const FabricCore::RTVal * args)
  {
    FabricCore::RTVal result;
    FECS_ConstructRTValArgs(result, rt, nbArgs, args);
    Exception::MaybeThrow();
    return result;
  }

  // creates a RTVal just given a KL object name
  inline FabricCore::RTVal constructObjectRTVal(const char * rt)
  {
    FabricCore::RTVal result;
    FECS_ConstructObjectRTVal(result, rt);
    Exception::MaybeThrow();
    return result;
  }

  // creates a RTVal given a KL object name and construction args
  inline FabricCore::RTVal constructObjectRTVal(const char * rt, uint32_t nbArgs, const FabricCore::RTVal * args)
  {
    FabricCore::RTVal result;
    FECS_ConstructObjectRTValArgs(result, rt, nbArgs, args);
    Exception::MaybeThrow();
    return result;
  }

  // creates a KL interface RTVal given a KL object to cast
  inline FabricCore::RTVal constructInterfaceRTVal(const char * rt, const FabricCore::RTVal & object)
  {
    FabricCore::RTVal result;
    FECS_ConstructInterfaceRTValArgs(result, rt, object);
    Exception::MaybeThrow();
    return result;
  }

  // creates a Boolean RTVal given its value
  inline FabricCore::RTVal constructBooleanRTVal(bool value)
  {
    FabricCore::RTVal result;
    FECS_ConstructBooleanRTVal(result, value);
    Exception::MaybeThrow();
    return result;
  }

  // creates a SInt8 RTVal given its value
  inline FabricCore::RTVal constructSInt8RTVal(int8_t value)
  {
    FabricCore::RTVal result;
    FECS_ConstructSInt8RTVal(result, value);
    Exception::MaybeThrow();
    return result;
  }

  // creates a SInt16 RTVal given its value
  inline FabricCore::RTVal constructSInt16RTVal(int16_t value)
  {
    FabricCore::RTVal result;
    FECS_ConstructSInt16RTVal(result, value);
    Exception::MaybeThrow();
    return result;
  }

  // creates a SInt32 RTVal given its value
  inline FabricCore::RTVal constructSInt32RTVal(int32_t value)
  {
    FabricCore::RTVal result;
    FECS_ConstructSInt32RTVal(result, value);
    Exception::MaybeThrow();
    return result;
  }

  // creates a SInt64 RTVal given its value
  inline FabricCore::RTVal constructSInt64RTVal(int64_t value)
  {
    FabricCore::RTVal result;
    FECS_ConstructSInt64RTVal(result, value);
    Exception::MaybeThrow();
    return result;
  }

  // creates a UInt8 RTVal given its value
  inline FabricCore::RTVal constructUInt8RTVal(uint8_t value)
  {
    FabricCore::RTVal result;
    FECS_ConstructUInt8RTVal(result, value);
    Exception::MaybeThrow();
    return result;
  }

  // creates a UInt16 RTVal given its value
  inline FabricCore::RTVal constructUInt16RTVal(uint16_t value)
  {
    FabricCore::RTVal result;
    FECS_ConstructUInt16RTVal(result, value);
    Exception::MaybeThrow();
    return result;
  }

  // creates a UInt32 RTVal given its value
  inline FabricCore::RTVal constructUInt32RTVal(uint32_t value)
  {
    FabricCore::RTVal result;
    FECS_ConstructUInt32RTVal(result, value);
    Exception::MaybeThrow();
    return result;
  }

  // creates a UInt64 RTVal given its value
  inline FabricCore::RTVal constructUInt64RTVal(uint64_t value)
  {
    FabricCore::RTVal result;
    FECS_ConstructUInt64RTVal(result, value);
    Exception::MaybeThrow();
    return result;
  }

  // creates a Float32 RTVal given its value
  inline FabricCore::RTVal constructFloat32RTVal(float value)
  {
    FabricCore::RTVal result;
    FECS_ConstructFloat32RTVal(result, value);
    Exception::MaybeThrow();
    return result;
  }

  // creates a Float64 RTVal given its value
  inline FabricCore::RTVal constructFloat64RTVal(double value)
  {
    FabricCore::RTVal result;
    FECS_ConstructFloat64RTVal(result, value);
    Exception::MaybeThrow();
    return result;
  }

  // creates a Float64 RTVal given its value
  inline FabricCore::RTVal constructDataRTVal(void *value)
  {
    FabricCore::RTVal result;
    FECS_ConstructDataRTVal(result, value);
    Exception::MaybeThrow();
    return result;
  }

  // creates a String RTVal given its value
  inline FabricCore::RTVal constructStringRTVal(const char * value)
  {
    FabricCore::RTVal result;
    FECS_ConstructStringRTVal(result, value);
    Exception::MaybeThrow();
    return result;
  }

  // creates a variable array RTVal given its type
  inline FabricCore::RTVal constructVariableArrayRTVal(const char * rt)
  {
    FabricCore::RTVal result;
    FECS_ConstructVariableArrayRTVal(result, rt);
    Exception::MaybeThrow();
    return result;
  }

  // creates an external array RTVal
  inline FabricCore::RTVal constructExternalArrayRTVal(const char * rt, uint32_t nbElements, void * data)
  {
    FabricCore::RTVal result;
    FECS_ConstructExternalArrayRTVal(result, rt, nbElements, data);
    Exception::MaybeThrow();
    return result;
  }

  class KLParser
  {
  public:
    class KLSymbol
    {
      friend class KLParserImpl;
      friend class KLConstant;
      friend class KLVariable;
      friend class KLStruct;
      friend class KLArgumentList;
      friend class KLOperator;
      friend class KLFunction;
      friend class KLInterface;

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

      friend class KLParser;
    public:
      // copy constructor
      KLSymbol(KLSymbol const & other)
      {
        mRef = other.mRef;
      }

      // copy operator
      KLSymbol & operator =( KLSymbol const & other )
      {
        mRef = other.mRef;
        return *this;
      }

      // returns true if the object is valid
      bool isValid() const
      {
        return mRef != NULL;
      }

      // bool conversion operator
	  operator explicit_bool::type() const
	  {
	    return explicit_bool::get(isValid());
	  }

      // returns the index of this symbol
      unsigned int index() const
      {
        unsigned int result = FECS_KLParser_KLSymbol_index(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the char position within the code
      unsigned int pos() const
      {
        unsigned int result = FECS_KLParser_KLSymbol_pos(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the length of this symbol
      unsigned int length() const
      {
        unsigned int result = FECS_KLParser_KLSymbol_length(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the type of this symbol
      Type type() const
      {
        Type result = (Type)FECS_KLParser_KLSymbol_type(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns true of this symbol is a keyword
      bool isKeyword() const
      {
        bool result = FECS_KLParser_KLSymbol_isKeyword(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns
      const char * typeName() const
      {
        const char * result = FECS_KLParser_KLSymbol_typeName(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns a char from the front (given an index)
      char front(unsigned int index = 0) const
      {
        char result = FECS_KLParser_KLSymbol_front(mRef, index);
        Exception::MaybeThrow();
        return result;
      }

      // returns a char from the back (given an index)
      char back(unsigned int index = 0) const
      {
        char result = FECS_KLParser_KLSymbol_back(mRef, index);
        Exception::MaybeThrow();
        return result;
      }

      // returns the contained string
      std::string str() const
      {
        char * resultC = FECS_KLParser_KLSymbol_str(mRef);
        Exception::MaybeThrow();
        std::string result;
        if(resultC)
        {
          result = resultC;
          FECS_Free(resultC);
        }
        return result;
      }

      // returns true if this contains a given char
      bool contains(char c) const
      {
        bool result = FECS_KLParser_KLSymbol_contains(mRef, c);
        Exception::MaybeThrow();
        return result;
      }

      // returns the index of a given char, or UINT_MAX if not found
      unsigned int find(char c) const
      {
        unsigned int result = FECS_KLParser_KLSymbol_find(mRef, c);
        Exception::MaybeThrow();
        return result;
      }

      // returns the previous symbol before this one, or an invalid one
      KLSymbol prev(bool skipComments = true, unsigned int offset = 1) const
      {
        FECS_KLParserSymbolRef ref = FECS_KLParser_KLSymbol_prev(mRef, skipComments, offset);
        Exception::MaybeThrow();
        return KLSymbol(ref);
      }

      // returns the next symbol after this one, or an invalid one
      KLSymbol next(bool skipComments = true, unsigned int offset = 1) const
      {
        FECS_KLParserSymbolRef ref = FECS_KLParser_KLSymbol_next(mRef, skipComments, offset);
        Exception::MaybeThrow();
        return KLSymbol(ref);
      }      

      // returns the name of the parser this symbol belongs to
      const char * parser() const
      {
        const char * result = FECS_KLParser_KLSymbol_parser(mRef);
        Exception::MaybeThrow();
        return result;
      }

    private:
      KLSymbol()
      { 
        mRef = NULL;
      }

      KLSymbol(FECS_KLParserSymbolRef ref)
      { 
        mRef = ref;
      }

      FECS_KLParserSymbolRef mRef;
    };

    class KLConstant
    {
      friend class KLParser;
    public:
      // copy constructor
      KLConstant(KLConstant const & other)
      {
        mRef = other.mRef;
      }

      // copy operator
      KLConstant & operator =( KLConstant const & other )
      {
        mRef = other.mRef;
        return *this;
      }

      // returns true if the object is valid
      bool isValid() const
      {
        return mRef != NULL;
      }

      // bool conversion operator
      operator explicit_bool::type() const
      {
        return explicit_bool::get(isValid());
      }

      // returns the symbol of this KLConstant
      KLSymbol symbol() const
      {
        FECS_KLParserSymbolRef result = FECS_KLParser_KLConstant_symbol(mRef);
        Exception::MaybeThrow();
        return KLSymbol(result);
      }

      // returns the comments of this KLConstant
      const char * comments() const
      {
        const char * result = FECS_KLParser_KLConstant_comments(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the type of this KLConstant
      const char * type() const
      {
        const char * result = FECS_KLParser_KLConstant_type(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the name of this KLConstant
      const char * name() const
      {
        const char * result = FECS_KLParser_KLConstant_name(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the value of this KLConstant
      const char * value() const
      {
        const char * result = FECS_KLParser_KLConstant_value(mRef);
        Exception::MaybeThrow();
        return result;
      }

    private:
      KLConstant()
      { 
        mRef = NULL;
      }

      KLConstant(FECS_KLParserConstantRef ref)
      { 
        mRef = ref;
      }

      FECS_KLParserConstantRef mRef;
    };

    class KLVariable
    {
      friend class KLParser;
    public:
      // copy constructor
      KLVariable(KLVariable const & other)
      {
        mRef = other.mRef;
      }

      // copy operator
      KLVariable & operator =( KLVariable const & other )
      {
        mRef = other.mRef;
        return *this;
      }

      // returns true if the object is valid
      bool isValid() const
      {
        return mRef != NULL;
      }

	  // bool conversion operator
	  operator explicit_bool::type() const
	  {
		  return explicit_bool::get(isValid());
	  }

      // returns the symbol of this KLVariable
      KLSymbol symbol() const
      {
        FECS_KLParserSymbolRef result = FECS_KLParser_KLVariable_symbol(mRef);
        Exception::MaybeThrow();
        return KLSymbol(result);
      }

      // returns the type of this KLVariable
      const char * type() const
      {
        const char * result = FECS_KLParser_KLVariable_type(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the name of this KLVariable
      const char * name() const
      {
        const char * result = FECS_KLParser_KLVariable_name(mRef);
        Exception::MaybeThrow();
        return result;
      }

    private:
      KLVariable()
      { 
        mRef = NULL;
      }

      KLVariable(FECS_KLParserVariableRef ref)
      { 
        mRef = ref;
      }

      FECS_KLParserVariableRef mRef;
    };

    class KLStruct
    {
      friend class KLParser;
    public:
      // copy constructor
      KLStruct(KLStruct const & other)
      {
        mRef = other.mRef;
      }

      // copy operator
      KLStruct & operator =( KLStruct const & other )
      {
        mRef = other.mRef;
        return *this;
      }

      // returns true if the object is valid
      bool isValid() const
      {
        return mRef != NULL;
      }

	  // bool conversion operator
	  operator explicit_bool::type() const
	  {
		  return explicit_bool::get(isValid());
	  }

      // returns the symbol of this KLStruct
      KLSymbol symbol() const
      {
        FECS_KLParserSymbolRef result = FECS_KLParser_KLStruct_symbol(mRef);
        Exception::MaybeThrow();
        return KLSymbol(result);
      }

      // returns the comments of this KLStruct
      const char * comments() const
      {
        const char * result = FECS_KLParser_KLStruct_comments(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the type of this KLStruct
      const char * type() const
      {
        const char * result = FECS_KLParser_KLStruct_type(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the name of this KLStruct
      const char * name() const
      {
        const char * result = FECS_KLParser_KLStruct_name(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns number of interfaces in this KLStruct
      unsigned int nbInterfaces() const
      {
        unsigned int result = FECS_KLParser_KLStruct_nbInterfaces(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns name of an interface of a given index
      const char * getInterface(unsigned int index) const
      {
        const char * result = FECS_KLParser_KLStruct_interface(mRef, index);
        Exception::MaybeThrow();
        return result;
      }

      // returns number of members in this KLStruct
      unsigned int nbMembers() const
      {
        unsigned int result = FECS_KLParser_KLStruct_nbMembers(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the type of a member of this KLStruct with a given index
      const char * memberType(unsigned int index) const
      {
        const char * result = FECS_KLParser_KLStruct_memberType(mRef, index);
        Exception::MaybeThrow();
        return result;
      }

      // returns the type of a member of this KLStruct with a given index
      const char * memberName(unsigned int index) const
      {
        const char * result = FECS_KLParser_KLStruct_memberName(mRef, index);
        Exception::MaybeThrow();
        return result;
      }

    private:
      KLStruct()
      { 
        mRef = NULL;
      }

      KLStruct(FECS_KLParserStructRef ref)
      { 
        mRef = ref;
      }

      FECS_KLParserStructRef mRef;
    };

    class KLArgumentList
    {
      friend class KLParser;
      friend class KLOperator;
      friend class KLFunction;
    public:
      // copy constructor
      KLArgumentList(KLArgumentList const & other)
      {
        mRef = other.mRef;
      }

      // copy operator
      KLArgumentList & operator =( KLArgumentList const & other )
      {
        mRef = other.mRef;
        return *this;
      }

      // returns true if the object is valid
      bool isValid() const
      {
        return mRef != NULL;
      }

	  // bool conversion operator
	  operator explicit_bool::type() const
	  {
		  return explicit_bool::get(isValid());
	  }

      // returns number of arguments in this KLArgumentList
      unsigned int nbArgs() const
      {
        unsigned int result = FECS_KLParser_KLArgumentList_nbArgs(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the mode of an argument with a given index
      const char * mode(unsigned int index) const
      {
        const char * result = FECS_KLParser_KLArgumentList_mode(mRef, index);
        Exception::MaybeThrow();
        return result;
      }

      // returns the type of an argument with a given index
      const char * type(unsigned int index) const
      {
        const char * result = FECS_KLParser_KLArgumentList_type(mRef, index);
        Exception::MaybeThrow();
        return result;
      }

      // returns the name of an argument with a given index
      const char * name(unsigned int index) const
      {
        const char * result = FECS_KLParser_KLArgumentList_name(mRef, index);
        Exception::MaybeThrow();
        return result;
      }

    private:
      KLArgumentList()
      { 
        mRef = NULL;
      }

      KLArgumentList(FECS_KLParserArgumentListRef ref)
      { 
        mRef = ref;
      }

      FECS_KLParserArgumentListRef mRef;
    };

    class KLOperator
    {
      friend class KLParser;
    public:
      // copy constructor
      KLOperator(KLOperator const & other)
      {
        mRef = other.mRef;
      }

      // copy operator
      KLOperator & operator =( KLOperator const & other )
      {
        mRef = other.mRef;
        return *this;
      }

      // returns true if the object is valid
      bool isValid() const
      {
        return mRef != NULL;
      }

	  // bool conversion operator
	  operator explicit_bool::type() const
	  {
		  return explicit_bool::get(isValid());
	  }

      // returns the symbol of this KLOperator
      KLSymbol symbol() const
      {
        FECS_KLParserSymbolRef result = FECS_KLParser_KLOperator_symbol(mRef);
        Exception::MaybeThrow();
        return KLSymbol(result);
      }

      // returns the comments of this KLOperator
      const char * comments() const
      {
        const char * result = FECS_KLParser_KLOperator_comments(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the name of this KLOperator
      const char * name() const
      {
        const char * result = FECS_KLParser_KLOperator_name(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns true if this KLOperator uses a PEX notation
      bool isPex() const
      {
        bool result = FECS_KLParser_KLOperator_isPex(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the pexArgument of this KLOperator
      const char * pexArgument() const
      {
        const char * result = FECS_KLParser_KLOperator_pexArgument(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the argument list of this KLOperator
      KLArgumentList arguments() const
      {
        FECS_KLParserArgumentListRef result = FECS_KLParser_KLOperator_arguments(mRef);
        Exception::MaybeThrow();
        return KLArgumentList(result);
      }

      // returns the symbol starting the body of this KLOperator
      KLSymbol bodyStart() const
      {
        FECS_KLParserSymbolRef result = FECS_KLParser_KLOperator_bodyStart(mRef);
        Exception::MaybeThrow();
        return KLSymbol(result);
      }

      // returns the symbol ending the body of this KLOperator
      KLSymbol bodyEnd() const
      {
        FECS_KLParserSymbolRef result = FECS_KLParser_KLOperator_bodyEnd(mRef);
        Exception::MaybeThrow();
        return KLSymbol(result);
      }

    private:
      KLOperator()
      { 
        mRef = NULL;
      }

      KLOperator(FECS_KLParserOperatorRef ref)
      { 
        mRef = ref;
      }

      FECS_KLParserOperatorRef mRef;
    };

    class KLFunction
    {
      friend class KLParser;
    public:
      // copy constructor
      KLFunction(KLFunction const & other)
      {
        mRef = other.mRef;
      }

      // copy operator
      KLFunction & operator =( KLFunction const & other )
      {
        mRef = other.mRef;
        return *this;
      }

      // returns true if the object is valid
      bool isValid() const
      {
        return mRef != NULL;
      }

	  // bool conversion operator
	  operator explicit_bool::type() const
	  {
		  return explicit_bool::get(isValid());
	  }

      // returns the symbol of this KLFunction
      KLSymbol symbol() const
      {
        FECS_KLParserSymbolRef result = FECS_KLParser_KLFunction_symbol(mRef);
        Exception::MaybeThrow();
        return KLSymbol(result);
      }

      // returns the comments of this KLFunction
      const char * comments() const
      {
        const char * result = FECS_KLParser_KLFunction_comments(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the type of this KLFunction (or "" if it is a void)
      const char * type() const
      {
        const char * result = FECS_KLParser_KLFunction_type(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the name of this KLFunction
      const char * name() const
      {
        const char * result = FECS_KLParser_KLFunction_name(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the owner of this KLFunction (or "" if it's not a method)
      const char * owner() const
      {
        const char * result = FECS_KLParser_KLFunction_owner(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the argument list of this KLFunction
      KLArgumentList arguments() const
      {
        FECS_KLParserArgumentListRef result = FECS_KLParser_KLFunction_arguments(mRef);
        Exception::MaybeThrow();
        return KLArgumentList(result);
      }

      // returns the symbol starting the body of this KLFunction
      KLSymbol bodyStart() const
      {
        FECS_KLParserSymbolRef result = FECS_KLParser_KLFunction_bodyStart(mRef);
        Exception::MaybeThrow();
        return KLSymbol(result);
      }

      // returns the symbol ending the body of this KLFunction
      KLSymbol bodyEnd() const
      {
        FECS_KLParserSymbolRef result = FECS_KLParser_KLFunction_bodyEnd(mRef);
        Exception::MaybeThrow();
        return KLSymbol(result);
      }

    private:
      KLFunction()
      { 
        mRef = NULL;
      }

      KLFunction(FECS_KLParserFunctionRef ref)
      { 
        mRef = ref;
      }

      FECS_KLParserFunctionRef mRef;
    };

    class KLInterface
    {
      friend class KLParser;
    public:
      // copy constructor
      KLInterface(KLInterface const & other)
      {
        mRef = other.mRef;
      }

      // copy operator
      KLInterface & operator =( KLInterface const & other )
      {
        mRef = other.mRef;
        return *this;
      }

      // returns true if the object is valid
      bool isValid() const
      {
        return mRef != NULL;
      }

	  // bool conversion operator
	  operator explicit_bool::type() const
	  {
		  return explicit_bool::get(isValid());
	  }

      // returns the symbol of this KLInterface
      KLSymbol symbol() const
      {
        FECS_KLParserSymbolRef result = FECS_KLParser_KLInterface_symbol(mRef);
        Exception::MaybeThrow();
        return KLSymbol(result);
      }

      // returns the comments of this KLInterface
      const char * comments() const
      {
        const char * result = FECS_KLParser_KLInterface_comments(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns the name of this KLInterface
      const char * name() const
      {
        const char * result = FECS_KLParser_KLInterface_name(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns number of functions in this KLInterface
      unsigned int nbFunctions() const
      {
        unsigned int result = FECS_KLParser_KLInterface_nbFunctions(mRef);
        Exception::MaybeThrow();
        return result;
      }

      // returns a function of this interface based on the index
      KLFunction function(unsigned int index) const
      {
        FECS_KLParserFunctionRef result = FECS_KLParser_KLInterface_function(mRef, index);
        Exception::MaybeThrow();
        return KLFunction(result);
      }

    private:
      KLInterface()
      { 
        mRef = NULL;
      }

      KLInterface(FECS_KLParserInterfaceRef ref)
      { 
        mRef = ref;
      }

      FECS_KLParserInterfaceRef mRef;
    };

    // default constructor
    KLParser()
    { 
      mRef = NULL;
    }

    // copy constructor
    KLParser(KLParser const & other)
    {
      mRef = FECS_KLParser_copy(other.mRef);
      Exception::MaybeThrow();
    }

    // copy operator
    KLParser & operator =( KLParser const & other )
    {
      FECS_KLParser_destroy(mRef);
      mRef = FECS_KLParser_copy(other.mRef);
      Exception::MaybeThrow();
      return *this;
    }

    ~KLParser()
    {
      FECS_KLParser_destroy(mRef);
      Exception::MaybeThrow();
    }

    // returns true if the object is valid
    bool isValid() const
    {
      return mRef != NULL;
    }

    // bool conversion operator
	operator explicit_bool::type() const
	{
	  return explicit_bool::get(isValid());
	}

    // returns the owner of the parser
    const char * owner() const 
    {
      const char * result = FECS_KLParser_owner(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the name of the parser
    const char * name() const 
    {
      const char * result = FECS_KLParser_name(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the contained sourcecode of the parser
    const char * code() const 
    {
      const char * result = FECS_KLParser_code(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the number of current parsers
    static unsigned int getNbParsers()
    {
      unsigned int result = FECS_KLParser_getNbParsers();
      Exception::MaybeThrow();
      return result;
    }

    // returns a parser given an index
    static KLParser getParser(unsigned int index)
    {
      FECS_KLParserRef ref = FECS_KLParser_getParserFromIndex(index);
      Exception::MaybeThrow();
      return KLParser(ref);
    }

    // returns a parser given a name an optional klCode
    // if the parser doesn't exist yet - it will be created.
    static KLParser getParser(const char * owner, const char * name, const char * klCode = NULL)
    {
      FECS_KLParserRef ref = FECS_KLParser_getParser(owner, name, klCode);
      Exception::MaybeThrow();
      return KLParser(ref);
    }

    // parse new KL code in this parser
    bool parse(const char * klCode)
    {
      bool result = FECS_KLParser_parse(mRef, klCode);
      Exception::MaybeThrow();
      return result;
    }

    // returns the number of KL symbols
    unsigned int getNbKLSymbols() const
    {
      unsigned int result = FECS_KLParser_getNbKLSymbols(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns a specific KL symbol
    KLSymbol getKLSymbol(unsigned int symbolIndex) const
    {
      FECS_KLParserSymbolRef result = FECS_KLParser_getKLSymbol(mRef, symbolIndex);
      Exception::MaybeThrow();
      return KLSymbol(result);
    }

    // returns the symbol for a special character index in the KL code
    KLSymbol getKLSymbolFromCharIndex(unsigned int charIndex) const
    {
      FECS_KLParserSymbolRef result = FECS_KLParser_getKLSymbolFromCharIndex(mRef, charIndex);
      Exception::MaybeThrow();
      return KLSymbol(result);
    }

    // returns the number of required KL types / extensions
    unsigned int getNbKLRequires() const
    {
      unsigned int result = FECS_KLParser_getNbKLRequires(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the name of the required KL type / extension given an index
    const char * getKLRequire(unsigned int index) const
    {
      const char * result = FECS_KLParser_getKLRequire(mRef, index);
      Exception::MaybeThrow();
      return result;
    }

    // returns the number of KL constants
    unsigned int getNbKLConstants() const
    {
      unsigned int result = FECS_KLParser_getNbKLConstants(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the KL constant given an index
    KLConstant getKLConstant(unsigned int index) const
    {
      FECS_KLParserConstantRef result = FECS_KLParser_getKLConstant(mRef, index);
      Exception::MaybeThrow();
      return KLConstant(result);
    }

    // returns the number of KL variables
    unsigned int getNbKLVariables() const
    {
      unsigned int result = FECS_KLParser_getNbKLVariables(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the KL variable given an index
    KLVariable getKLVariable(unsigned int index) const
    {
      FECS_KLParserVariableRef result = FECS_KLParser_getKLVariable(mRef, index);
      Exception::MaybeThrow();
      return KLVariable(result);
    }

    // returns the number of KL interfaces
    unsigned int getNbKLInterfaces() const
    {
      unsigned int result = FECS_KLParser_getNbKLInterfaces(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the KL interface given an index
    KLInterface getKLInterface(unsigned int index) const
    {
      FECS_KLParserInterfaceRef result = FECS_KLParser_getKLInterface(mRef, index);
      Exception::MaybeThrow();
      return KLInterface(result);
    }

    // returns the number of KL structs / objects
    unsigned int getNbKLStructs() const
    {
      unsigned int result = FECS_KLParser_getNbKLStructs(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the KL struct / object given an index
    KLStruct getKLStruct(unsigned int index) const
    {
      FECS_KLParserStructRef result = FECS_KLParser_getKLStruct(mRef, index);
      Exception::MaybeThrow();
      return KLStruct(result);
    }

    // returns the number of KL operators
    unsigned int getNbKLOperators() const
    {
      unsigned int result = FECS_KLParser_getNbKLOperators(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the KL operator given an index
    KLOperator getKLOperator(unsigned int index) const
    {
      FECS_KLParserOperatorRef result = FECS_KLParser_getKLOperator(mRef, index);
      Exception::MaybeThrow();
      return KLOperator(result);
    }

    // returns the number of KL functions / methods
    unsigned int getNbKLFunctions() const
    {
      unsigned int result = FECS_KLParser_getNbKLFunctions(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the KL function given an index
    KLFunction getKLFunction(unsigned int index) const
    {
      FECS_KLParserFunctionRef result = FECS_KLParser_getKLFunction(mRef, index);
      Exception::MaybeThrow();
      return KLFunction(result);
    }

    // returns the KL type for a given name symbol (or "" if unknown)
    const char * getKLTypeForSymbol(const KLSymbol & symbol) const
    {
      const char * result = FECS_KLParser_getKLTypeForSymbol(mRef, symbol.mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the KL type for a member or method below a given owner type
    static const char * getKLTypeForSymbol(const char * owner, const char * member)
    {
      const char * result = FECS_KLParser_getKLTypeForMemberOrMethod(owner, member);
      Exception::MaybeThrow();
      return result;
    }

  private:

    KLParser(FECS_KLParserRef ref)
    { 
      mRef = ref;
    }

    FECS_KLParserRef mRef;
  };

  class Logging
  {
  public:
    // sets the callback for generic log messages
    static void setLogFunc(LoggingFunc func)
    {
      FECS_Logging_setLogFunc(func);
    }

    // sets the callback for error log messages
    static void setLogErrorFunc(LoggingFunc func)
    {
      FECS_Logging_setLogErrorFunc(func); 
    }

    // sets the callback for KL compiler error messages
    static void setCompilerErrorFunc(CompilerErrorFunc func)
    {
      FECS_Logging_setCompilerErrorFunc(func); 
    }

    // sets the callback for KL report statements
    static void setKLReportFunc(LoggingFunc func)
    {
      FECS_Logging_setKLReportFunc(func); 
    }

    // sets the callback for KL queueStatusMessage statements
    static void setKLStatusFunc(StatusFunc func)
    {
      FECS_Logging_setKLStatusFunc(func); 
    }

    // sets the callback for KL queueStatusMessage statements
    static void setSlowOperationFunc(SlowOperationFunc func)
    {
      FECS_Logging_setSlowOperationFunc(func); 
    }

    // enable timers
    static void enableTimers()
    {
      FECS_Logging_enableTimers(); 
    }

    // disable timers
    static void disableTimers()
    {
      FECS_Logging_disableTimers(); 
    }

    // reset a timer
    static void resetTimer(const char * name)
    {
      FECS_Logging_resetTimer(name);
    }

    // start a timer
    static void startTimer(const char * name)
    {
      FECS_Logging_startTimer(name);
    }

    // stop a timer and accumulate the time
    static void stopTimer(const char * name)
    {
      FECS_Logging_stopTimer(name);
    }

    // log a given timer
    static void logTimer(const char * name)
    {
      FECS_Logging_logTimer(name);
    }      

    // return the number of existing timers
    static unsigned int getNbTimers()
    {
      return FECS_Logging_getNbTimers();
    }

    // return the name of a specific timer
    static char const * getTimerName(unsigned int index)
    {
      return FECS_Logging_getTimerName(index);
    }

    // a timer which records time on construction and destruction
    class AutoTimer
    {
    public:
      AutoTimer(std::string const &name)
      {
        mName = name;
        Logging::startTimer(mName.c_str());
      }

      ~AutoTimer()
      {
        Logging::stopTimer(mName.c_str());
      }

      const char * getName()
      {
        return mName.c_str();
      }

      void resume()
      {
        startTimer(getName());
      }

      void stop()
      {
        stopTimer(getName());
      }

    private:
      std::string mName;
    };        
  };

  class DGPort
  {
    friend class DGGraph;
    friend class SceneManagement;

  public:
    
    DGPort()
    { 
      mRef = NULL;
    }

    DGPort(DGPort const & other)
    {
      mRef = FECS_DGPort_copy(other.mRef);
      Exception::MaybeThrow();
    }

    DGPort & operator =( DGPort const & other )
    {
      FECS_DGPort_destroy(mRef);
      mRef = FECS_DGPort_copy(other.mRef);
      Exception::MaybeThrow();
      return *this;
    }

    ~DGPort()
    {
      FECS_DGPort_destroy(mRef);
      Exception::MaybeThrow();
    }

    // returns true if the object is valid
    bool isValid() const
    {
      return mRef != NULL;
    }

	// bool conversion operator
	// returning bool_type prevents the automatic
	// conversion to int
	operator explicit_bool::type() const
	{
		return explicit_bool::get(isValid());
	}

    // empties the content of the port
    void clear()
    {
      FECS_DGGraph_clear(mRef);
      Exception::MaybeThrow();
    }

    /*
      Basic data getters
    */

    // returns the name of the member this DGPort is connected to
    char const * getName()
    {
      char const * result = FECS_DGPort_getName(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the name of the member this DGPort is connected to
    char const * getMember()
    {
      char const * result = FECS_DGPort_getMember(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the name of the DGNode this DGPort is connected to
    char const * getDGNodeName()
    {
      char const * result = FECS_DGPort_getDGNodeName(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns a unique key descripting the Port
    char const * getKey()
    {
      char const * result = FECS_DGPort_getKey(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the mode of this DGPort
    Port_Mode getMode()
    {
      Port_Mode result = (Port_Mode)FECS_DGPort_getMode(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // sets the mode of this DGPort
    void setMode(Port_Mode mode)
    {
      FECS_DGPort_setMode(mRef, mode);
      Exception::MaybeThrow();
    }

    // returns the data type of the member this DGPort is connected to
    char const * getDataType()
    {
      char const * result = FECS_DGPort_getDataType(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the data size of a single element of the member this DGPort is connected to.
    // So for example, both for a 'Vec3' and 'Vec3[]' this will return sizeof(Vec3) == 12
    unsigned int getDataSize()
    {
      unsigned int result = FECS_DGPort_getDataSize(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns true if the data type of this DGPort is shallow.
    // only shallow data types can be used with the high performance IO
    bool isShallow()
    {
      bool result = FECS_DGPort_isShallow(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns true if the data type of this DGPort is an array (Vec3[] for example)
    bool isArray()
    {
      bool result = FECS_DGPort_isArray(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns true if the data type of this DGPort is a struct
    bool isStruct()
    {
      bool result = FECS_DGPort_isStruct(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns true if the data type of this DGPort is an object
    bool isObject()
    {
      bool result = FECS_DGPort_isObject(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns true if the data type of this DGPort is an interface
    bool isInterface()
    {
      bool result = FECS_DGPort_isInterface(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns true if this port auto initializes KL objects
    bool doesAutoInitObjects()
    {
      bool result = FECS_DGPort_doesAutoInitObjects(mRef);
      Exception::MaybeThrow();
      return result;
    }

    /*
      Basic Data Setters
    */

    /*
      FabricCore slicing management
    */

    // returns the slice count of the FabricCore::DGNode this DGPort is connected to
    unsigned int getSliceCount()
    {
      unsigned int result = FECS_DGPort_getSliceCount(mRef); 
      Exception::MaybeThrow();
      return result;
    }

    // sets the slice count of the FabricCore::DGNode this DGPort is connected to
    bool setSliceCount(unsigned int count)
    {
      bool result = FECS_DGPort_setSliceCount(mRef, count); 
      Exception::MaybeThrow();
      return result;
    }

    /*
      FabricCore::Variant IO
    */

    // returns the value of a specific slice of this DGPort as a FabricCore::Variant
    FabricCore::Variant getVariant(unsigned int slice = 0)
    {
      FabricCore::Variant result;
      FECS_DGPort_getVariant(mRef, slice, result);
      Exception::MaybeThrow();
      return result;
    }

    // sets the value of a specific slice of this DGPort from a FabricCore::Variant
    bool setVariant(FabricCore::Variant value, unsigned int slice = 0)
    {
      bool result = FECS_DGPort_setVariant(mRef, value, slice);
      Exception::MaybeThrow();
      return result;
    }

    // returns the value of a specific slice of this DGPort as a FabricCore::RTVal
    FabricCore::RTVal getRTVal(bool evaluate = false, uint32_t slice = 0)
    {
      FabricCore::RTVal result;
      FECS_DGPort_getRTVal(mRef, evaluate, slice, result);
      Exception::MaybeThrow();
      return result;
    }

    // sets the value of a specific slice of this DGPort from a FabricCore::RTVal
    bool setRTVal(FabricCore::RTVal value, uint32_t slice = 0)
    {
      bool result = FECS_DGPort_setRTVal(mRef, value, slice);
      Exception::MaybeThrow();
      return result;
    }

    // returns the value of a specific slice of this DGPort as a JSON string
    std::string getJSON(unsigned int slice = 0)
    {
      char * ptr = FECS_DGPort_getJSON(mRef, slice);
      Exception::MaybeThrow();
      if(ptr == NULL)
        return "";
      std::string result = ptr;
      FECS_Free(ptr);
      return result;
    }

    // sets the value of a specific slice of this DGPort from a JSON string
    bool setJSON(const char * json, unsigned int slice = 0)
    {
      bool result = FECS_DGPort_setJSON(mRef, json, slice);
      Exception::MaybeThrow();
      return result;
    }

    // returns the default value of this DGPort as a FabricCore::Variant
    FabricCore::Variant getDefault()
    {
      FabricCore::Variant result;
      FECS_DGPort_getDefault(mRef, result);
      Exception::MaybeThrow();
      return result;
    }

    /*
      High Performance IO
      void* access to the internal data of the FabricCore::DGNode is only possible
      for shallow data types (like Vec3 for example).
    */

    // returns the size of an array member this DGPort is connected to
    unsigned int getArrayCount(unsigned int slice = 0)
    {
      unsigned int result = FECS_DGPort_getArrayCount(mRef, slice);
      Exception::MaybeThrow();
      return result;
    }

    // returns the void* array data of this DGPort.
    // this only works for array Ports (isArray() == true)
    // the bufferSize has to match getArrayCount() * getDataSize()
    bool getArrayData(
      void * buffer,
      unsigned int bufferSize,
      unsigned int slice = 0
      )
    {
      bool result = FECS_DGPort_getArrayData(mRef, buffer, bufferSize, slice);
      Exception::MaybeThrow();
      return result;
    }

    // sets the void* array data of this DGPort.
    // this only works for array Ports (isArray() == true)
    // this also sets the array count determined by bufferSize / getDataSize()
    bool setArrayData(void * buffer, unsigned int bufferSize, unsigned int slice = 0)
    {
      bool result = FECS_DGPort_setArrayData(mRef, buffer, bufferSize, slice);
      Exception::MaybeThrow();
      return result;
    }

    // gets the void* slice array data of this DGPort.
    // this only works for non-array Ports (isArray() == false)
    // the bufferSize has to match getSliceCount() * getDataSize()
    bool getAllSlicesData(void * buffer, unsigned int bufferSize)
    {
      bool result = FECS_DGPort_getAllSlicesData(mRef, buffer, bufferSize);
      Exception::MaybeThrow();
      return result;
    }

    // sets the void* slice array data of this DGPort.
    // this only works for non-array Ports (isArray() == false)
    // the bufferSize has to match getSliceCount() * getDataSize()
    bool setAllSlicesData(void * buffer, unsigned int bufferSize)
    {
      bool result = FECS_DGPort_setAllSlicesData(mRef, buffer, bufferSize);
      Exception::MaybeThrow();
      return result;
    }

    // set the array data based on another port
    // this performs data replication, and only works on shallow array data ports.
    // the data type has to match as well (so only Vec3 to Vec3 for example).
    bool copyArrayDataFromDGPort(DGPort other, unsigned int slice = 0, unsigned int otherSlice = UINT_MAX)
    {
      bool result = FECS_DGPort_copyArrayDataFromPort(mRef, other.mRef, slice, otherSlice);
      Exception::MaybeThrow();
      return result;
    }

    // set the slices data based on another port
    // this performs data replication, and only works on shallow non array data ports.
    // the data type has to match as well (so only Vec3 to Vec3 for example).
    bool copyAllSlicesDataFromDGPort(DGPort other, bool resizeTarget = false)
    {
      bool result = FECS_DGPort_copyAllSlicesDataFromPort(mRef, other.mRef, resizeTarget);
      Exception::MaybeThrow();
      return result;
    }

    // sets an auxiliary option
    void setOption(const char * name, const FabricCore::Variant & value)
    {
      FECS_DGPort_setOption(mRef, name, value);
      Exception::MaybeThrow();
    }

    // gets an auxiliary option (-1 if not defined)
    FabricCore::Variant getOption(const char * name)
    {
      FabricCore::Variant option;
      FECS_DGPort_getOption(mRef, name, option);
      Exception::MaybeThrow();
      return option;
    }

    // returns true if an auxiliary option exists
    bool hasOption(const char * name)
    {
      FabricCore::Variant option = getOption(name);
      return !option.isNull();
    }

    // gets an auxiliary option as boolean
    bool getBooleanOption(const char * name, bool defaultValue = false)
    {
      FabricCore::Variant option = getOption(name);
      if(!option.isBoolean())
        return getIntegerOption(name, defaultValue) != 0;
      return option.getBoolean();
    }

    // gets an auxiliary option as int
    int getIntegerOption(const char * name, int defaultValue = -1)
    {
      FabricCore::Variant option = getOption(name);
      if(option.isSInt8())
        return (int)option.getSInt8();
      if(option.isSInt16())
        return (int)option.getSInt16();
      if(option.isSInt32())
        return (int)option.getSInt32();
      if(option.isSInt64())
        return (int)option.getSInt64();
      if(option.isUInt8())
        return (int)option.getUInt8();
      if(option.isUInt16())
        return (int)option.getUInt16();
      if(option.isUInt32())
        return (int)option.getUInt32();
      if(option.isUInt64())
        return (int)option.getUInt64();
      if(option.isFloat32())
        return (int)option.getFloat32();
      if(option.isFloat64())
        return (int)option.getFloat64();
      return defaultValue;
    }

    // gets an auxiliary option as float
    float getScalarOption(const char * name, float defaultValue = 0.0f)
    {
      FabricCore::Variant option = getOption(name);
      if(option.isSInt8())
        return (float)option.getSInt8();
      if(option.isSInt16())
        return (float)option.getSInt64();
      if(option.isSInt32())
        return (float)option.getSInt32();
      if(option.isSInt64())
        return (float)option.getSInt64();
      if(option.isUInt8())
        return (float)option.getUInt8();
      if(option.isUInt16())
        return (float)option.getUInt16();
      if(option.isUInt32())
        return (float)option.getUInt32();
      if(option.isUInt64())
        return (float)option.getUInt64();
      if(option.isFloat32())
        return (float)option.getFloat32();
      if(option.isFloat64())
        return (float)option.getFloat64();
      return defaultValue;
    }

    // gets an auxiliary option as string
    std::string getStringOption(const char * name, const char * defaultValue = "")
    {
      FabricCore::Variant option = getOption(name);
      if(option.isString())
        return option.getStringData();
      return defaultValue;
    }

    /*
      Scene Management
    */

    // returns true if this port can be manipulated
    // bool isManipulatable() const
    // {
    //   bool result = FECS_DGPort_isManipulatable(mRef);
    //   Exception::MaybeThrow();
    //   return result;
    // }

    // returns all animation channels of this port
    // FabricCore::RTVal getAnimationChannels()
    // {
    //   FabricCore::RTVal result;
    //   FECS_DGPort_getAnimationChannels(mRef, result);
    //   Exception::MaybeThrow();
    //   return result;
    // }

    // // sets the values of all animation channels
    // void setAnimationChannelValues(unsigned int nbChannels, float * values)
    // {
    //   FECS_DGPort_setAnimationChannelValues(mRef, nbChannels, values);
    //   Exception::MaybeThrow();
    // }

    // // perform a manipulation
    // int manipulate(FabricCore::RTVal & manipulationContext)
    // {
    //   int result = FECS_DGPort_manipulate(mRef, manipulationContext);
    //   Exception::MaybeThrow();
    //   return result;
    // }

  private:
    DGPort(FECS_DGPortRef ref)
    { 
      mRef = ref;
    }
    FECS_DGPortRef mRef;
  };

  class DGGraph
  {
  public:
    
    DGGraph()
    { 
      mRef = NULL;
    }

    DGGraph(const char * name, int guarded = 1, FabricCore::ClientOptimizationType optType = FabricCore::ClientOptimizationType_Background)
    { 
      mRef = FECS_DGGraph_construct(name, guarded, optType); 
      Exception::MaybeThrow();
    }

    DGGraph(DGGraph const & other)
    {
      mRef = FECS_DGGraph_copy(other.mRef);
      Exception::MaybeThrow();
    }

    DGGraph & operator =( DGGraph const & other )
    {
      FECS_DGGraph_destroy(mRef);
      mRef = FECS_DGGraph_copy(other.mRef);
      Exception::MaybeThrow();
      return *this;
    }

    ~DGGraph()
    {
      FECS_DGGraph_destroy(mRef);
      Exception::MaybeThrow();
    }

    // returns true if the object is valid
    bool isValid() const
    {
      return mRef != NULL;
    }

    // bool conversion operator
	operator explicit_bool::type() const
	{
	  return explicit_bool::get(isValid());
	}

    // empties the content of the graph
    void clear()
    {
      FECS_DGGraph_clear(mRef);
      Exception::MaybeThrow();
    }

    // returns the FabricCore client
    static const FabricCore::Client * getClient()
    {
      const FabricCore::Client * result = NULL;
      FECS_DGGraph_getClient(&result);
      Exception::MaybeThrow();
      return result;
    }

    // loads the given extension
    static void loadExtension(const char *extensionName)
    {
      FECS_DGGraph_loadExtension(extensionName);
      Exception::MaybeThrow();
    }

    // retrieve the user pointer
    void * getUserPointer()
    {
      void * result = FECS_DGGraph_getUserPointer(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // store a user pointer
    void setUserPointer(void * data)
    {
      FECS_DGGraph_setUserPointer(mRef, data);
      Exception::MaybeThrow();
    }

    // returns the name of this graph
    const char * getName()
    {
      const char * result = FECS_DGGraph_getName(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // sets the name and ensures name uniqueness
    bool setName(const char * name)
    {
      bool result = FECS_DGGraph_setName(mRef, name);
      Exception::MaybeThrow();
      return result;
    }

    /*
      Metadata
    */

    // retrieves the metadata from the DGGraph
    const char * getMetaData()
    {
      const char * result = FECS_DGGraph_getMetaData(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // sets metadata on the DGGraph
    void setMetaData(const char * json)
    {
      FECS_DGGraph_setMetaData(mRef, json);
      Exception::MaybeThrow();
    }

    /*
      DG node management
    */

    // returns the number of DGNodes on this graph
    unsigned int getDGNodeCount()
    {
      unsigned int result = FECS_DGGraph_getDGNodeCount(mRef);
      Exception::MaybeThrow();
      return result;
    }
    
    // returns the name of a specific DGNode in this graph
    char const * getDGNodeName(unsigned int index = 0)
    {
      char const * result = FECS_DGGraph_getDGNodeName(mRef, index);
      Exception::MaybeThrow();
      return result;
    }

    // adds a member based on a member name and type (rt)
    bool addDGNodeMember(const char * name, const char * rt, FabricCore::Variant defaultValue = FabricCore::Variant(), const char * dgNodeName = "", const char * extension = "")
    {
      bool result = FECS_DGGraph_addDGNodeMember(mRef, name, rt, defaultValue, dgNodeName, extension);
      Exception::MaybeThrow();
      return result;
    }
    
    // returns true if a specific member exists
    bool hasDGNodeMember(const char * name, const char * dgNodeName = "")
    {
      bool result = FECS_DGGraph_hasDGNodeMember(mRef, name, dgNodeName);
      Exception::MaybeThrow();
      return result;
    }

    // removes a member
    bool removeDGNodeMember(const char * name, const char * dgNodeName = "")
    {
      bool result = FECS_DGGraph_removeDGNodeMember(mRef, name, dgNodeName);
      Exception::MaybeThrow();
      return result;
    }

    /*
      DG operator management
    */

    // returns KL source code for the parameter list for all available ports
    FabricCore::Variant generateKLOperatorParameterList()
    {
      FabricCore::Variant code;
      FECS_DGGraph_generateKLOperatorParameterList(mRef, code);
      Exception::MaybeThrow();
      return code;
    }

    // returns dummy KL source code for a new operator
    FabricCore::Variant generateKLOperatorSourceCode(const char * name, const char * additionalBody = "", const char * additionalFunctions = "", const char * executeParallelMember = "")
    {
      FabricCore::Variant code;
      FECS_DGGraph_generateKLOperatorSourceCode(mRef, code, name, additionalBody, additionalFunctions, executeParallelMember);
      Exception::MaybeThrow();
      return code;
    }

    // constructs a FabricCore::DGOperator based on a name and a kl source string
    bool constructKLOperator(const char * name, const char * sourceCode = "", const char * entry = "", const char * dgNodeName = "", const FabricCore::Variant & portMap = FabricCore::Variant::CreateDict())
    {
      bool result = FECS_DGGraph_constructKLOperator(mRef, name, sourceCode, entry, dgNodeName, portMap);
      Exception::MaybeThrow();
      return result;
    }

    // removes a KL operator from this Node
    bool removeKLOperator(const char * name, const char * dgNodeName = "")
    {
      bool result = FECS_DGGraph_removeKLOperator(mRef, name, dgNodeName);
      Exception::MaybeThrow();
      return result;
    }

    // returns true if this graph contains a given KL operator
    bool hasKLOperator(const char * name, const char * dgNodeName = "")
    {
      bool result = FECS_DGGraph_hasKLOperator(mRef, name, dgNodeName);
      Exception::MaybeThrow();
      return result;
    }

    // gets the entry point of a specific FabricCore::DGOperator
    char const * getKLOperatorEntry(const char * name)
    {
      char const * result = FECS_DGGraph_getKLOperatorEntry(mRef, name);
      Exception::MaybeThrow();
      return result;
    }

    // sets the entry point of a specific FabricCore::DGOperator
    bool setKLOperatorEntry(const char * name, const char * entry)
    {
      bool result = FECS_DGGraph_setKLOperatorEntry(mRef, name, entry);
      Exception::MaybeThrow();
      return result;
    }

    // moves the FabricCore::DGOperator on the stack to a given index
    bool setKLOperatorIndex(const char * name, unsigned int index)
    {
      bool result = FECS_DGGraph_setKLOperatorIndex(mRef, name, index);
      Exception::MaybeThrow();
      return result;
    }

    // gets the source code of a specific FabricCore::DGOperator
    char const * getKLOperatorSourceCode(const char * name)
    {
      char const * result = FECS_DGGraph_getKLOperatorSourceCode(mRef, name);
      Exception::MaybeThrow();
      return result;
    }

    // sets the source code of a specific FabricCore::DGOperator
    bool setKLOperatorSourceCode(const char * name, const char * sourceCode, const char * entry = "")
    {
      bool result = FECS_DGGraph_setKLOperatorSourceCode(mRef, name, sourceCode, entry);
      Exception::MaybeThrow();
      return result;
    }

    // loads the source code of a specific FabricCore::DGOperator from file
    void loadKLOperatorSourceCode(const char * name, const char * filePath)
    {
      FECS_DGGraph_loadKLOperatorSourceCode(mRef, name, filePath);
      Exception::MaybeThrow();
    }

    // saves the source code of a specific FabricCore::DGOperator to file
    void saveKLOperatorSourceCode(const char * name, const char * filePath)
    {
      FECS_DGGraph_saveKLOperatorSourceCode(mRef, name, filePath);
      Exception::MaybeThrow();
    }

    // returns true if the KL operator is using a file
    bool isKLOperatorFileBased(const char * name)
    {
      bool result = FECS_DGGraph_isKLOperatorFileBased(mRef, name);
      Exception::MaybeThrow();
      return result;
    }

    // gets the filepath of a specific FabricCore::DGOperator
    char const * getKLOperatorFilePath(const char * name)
    {
      char const * result = FECS_DGGraph_getKLOperatorFilePath(mRef, name);
      Exception::MaybeThrow();
      return result;
    }

    // loads the content of the file and sets the code
    void setKLOperatorFilePath(const char * name, const char * filePath, const char * entry = NULL)
    {
      FECS_DGGraph_setKLOperatorFilePath(mRef, name, filePath, entry);
      Exception::MaybeThrow();
    }

    // returns the number of operators in this graph
    unsigned int getKLOperatorCount(const char * dgNodeName = "")
    {
      unsigned int result = FECS_DGGraph_getKLOperatorCount(mRef, dgNodeName);
      Exception::MaybeThrow();
      return result;
    }

    // returns the name of a specific operator in this graph
    char const * getKLOperatorName(unsigned int index = 0, const char * dgNodeName = "")
    {
      char const * result = FECS_DGGraph_getKLOperatorName(mRef, index, dgNodeName);
      Exception::MaybeThrow();
      return result;
    }

    // returns the number of operators in total
    static unsigned int getGlobalKLOperatorCount()
    {
      unsigned int result = FECS_DGGraph_getGlobalKLOperatorCount();
      Exception::MaybeThrow();
      return result;
    }

    // returns the name of a specific operator
    static char const * getGlobalKLOperatorName(unsigned int index = 0)
    {
      char const * result = FECS_DGGraph_getGlobalKLOperatorName(index);
      Exception::MaybeThrow();
      return result;
    }

    /*
      Dependency graph evaluation
    */

    // checks all FabricCore::DGNodes and FabricCore::Operators for errors, return false if any errors found
    static bool checkErrors()
    {
      bool result = FECS_DGGraph_checkErrors();
      Exception::MaybeThrow();
      return result;
    }

    // whether to evaluate the node using a shared lock
    void setEvaluateShared( bool evaluateShared )
    {
      FECS_DGGraph_setEvaluateShared(mRef, evaluateShared);
      Exception::MaybeThrow();
    }

    // evaluates the contained DGNode
    bool evaluate()
    {
      bool result = FECS_DGGraph_evaluate(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // clears the evaluate state
    bool clearEvaluate()
    {
      bool result = FECS_DGGraph_clearEvaluate(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns if this graph is using the eval context
    bool usesEvalContext()
    {
      bool result = FECS_DGGraph_usesEvalContext(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // requires the evaluate to take place
    bool requireEvaluate()
    {
      bool result = FECS_DGGraph_requireEvaluate(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the graph's evaluation context
    FabricCore::RTVal getEvalContext()
    {
      FabricCore::RTVal result;
      FECS_DGGraph_getEvalContext(mRef, result);
      Exception::MaybeThrow();
      return result;
    }


    /*
      DGPort management
    */

    // adds a new DGPort provided a name, the member and a mode
    DGPort addDGPort(const char * name, const char * member, FabricSplice::Port_Mode mode, const char * dgNodeName = "", bool autoInitObjects = true)
    {
      FECS_DGPortRef result = FECS_DGGraph_addDGPort(mRef, name, member, (FECS_DGPort_Mode)mode, dgNodeName, autoInitObjects);
      Exception::MaybeThrow();
      return DGPort(result);
    }

    // removes an existing DGPort by name
    bool removeDGPort(const char * name)
    {
      bool result = FECS_DGGraph_removeDGPort(mRef, name);
      Exception::MaybeThrow();
      return result;
    }

    // returns a specific DGPort by name
    DGPort getDGPort(const char * name)
    {
      FECS_DGPortRef result = FECS_DGGraph_getDGPort(mRef, name);
      Exception::MaybeThrow();
      return DGPort(result);
    }

    // returns a specific Port by index
    DGPort getDGPort(unsigned int index)
    {
      FECS_DGPortRef result = FECS_DGGraph_getDGPortByIndex(mRef, index);
      Exception::MaybeThrow();
      return DGPort(result);
    }

    // returns the number of ports in this graph
    unsigned int getDGPortCount()
    {
      unsigned int result = FECS_DGGraph_getDGPortCount(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the name of a specific port in this graph
    char const * getDGPortName(unsigned int index)
    {
      char const * result = FECS_DGGraph_getDGPortName(mRef, index);
      Exception::MaybeThrow();
      return result;
    }

    // returns JSON string encoding the port layout of the node
    std::string getDGPortInfo()
    {
      char * resultPtr = FECS_DGGraph_getDGPortInfo(mRef);;
      Exception::MaybeThrow();
      if(resultPtr == NULL)
        return "";
      std::string result = resultPtr;
      FECS_Free(resultPtr);
      return result;
    }

    /*
      Subgraph management
    */

    // returns true if a DGNode of the given name exists
    bool hasDGNode(const char * dgNodeName)
    {
      bool result = FECS_DGGraph_hasDGNode(mRef, dgNodeName);
      Exception::MaybeThrow();
      return result;
    }

    // returns the internal FabricCore::DGNode based on an index
    FabricCore::DGNode getDGNode(unsigned int index = 0)
    {
      FabricCore::DGNode dgNode;
      FECS_DGGraph_getDGNodeByIndex(mRef, dgNode, index);
      Exception::MaybeThrow();
      return dgNode;
    }

    // returns the internal FabricCore::DGNode
    FabricCore::DGNode getDGNode(const char * dgNodeName = "")
    {
      FabricCore::DGNode dgNode;
      FECS_DGGraph_getDGNode(mRef, dgNode, dgNodeName);
      Exception::MaybeThrow();
      return dgNode;
    }

    // creates a new DG node
    void constructDGNode(const char * dgNodeName = "")
    {
      FECS_DGGraph_constructDGNode(mRef, dgNodeName);
      Exception::MaybeThrow();
    }

    // removes an existing DG node
    void removeDGNode(const char * dgNodeName)
    {
      FECS_DGGraph_removeDGNode(mRef, dgNodeName);
      Exception::MaybeThrow();
    }

    // returns true if a given DG node is dependent on another one
    bool hasDGNodeDependency(const char * dgNode, const char * dependency)
    {
      bool result = FECS_DGGraph_hasDGNodeDependency(mRef, dgNode, dependency);
      Exception::MaybeThrow();
      return result;
    }

    // depends one DGNode on another one
    bool setDGNodeDependency(const char * dgNode, const char * dependency)
    {
      bool result = FECS_DGGraph_setDGNodeDependency(mRef, dgNode, dependency);
      Exception::MaybeThrow();
      return result;
    }

    // removes the dependency of one DGNode on another one
    bool removeDGNodeDependency(const char * dgNode, const char * dependency)
    {
      bool result = FECS_DGGraph_removeDGNodeDependency(mRef, dgNode, dependency);
      Exception::MaybeThrow();
      return result;
    }

    /*
      Persistence management
    */

    // returns variant dict of the persistence data of a node
    FabricCore::Variant getPersistenceDataDict(const PersistenceInfo * info = NULL)
    {
      FabricCore::Variant data;
      FECS_DGGraph_getPersistenceDataDict(mRef, data, info);
      Exception::MaybeThrow();
      return data;
    }

    // returns JSON string encoding of the persistence data of a node
    std::string getPersistenceDataJSON(const PersistenceInfo * info = NULL)
    {
      char * ptr = FECS_DGGraph_getPersistenceDataJSON(mRef, info);
      Exception::MaybeThrow();
      if(ptr == NULL)
        return "";
      std::string data = ptr;
      FECS_Free(ptr);
      return data;
    }

    // constructs the node based on a variant dict
    bool setFromPersistenceDataDict(const FabricCore::Variant & dict, PersistenceInfo * info = NULL, const char * baseFilePath = NULL)
    {
      bool result = FECS_DGGraph_setFromPersistenceDataDict(mRef, dict, info, baseFilePath);
      Exception::MaybeThrow();
      return result;
    }

    // constructs the node based on a JSON string
    bool setFromPersistenceDataJSON(const std::string & json, PersistenceInfo * info = NULL, const char * baseFilePath = NULL)
    {
      bool result = FECS_DGGraph_setFromPersistenceDataJSON(mRef, json.c_str(), info, baseFilePath);
      Exception::MaybeThrow();
      return result;
    }
    
    // persists the node description into a JSON file
    bool saveToFile(const char * filePath, const PersistenceInfo * info = NULL)
    {
      bool result = FECS_DGGraph_saveToFile(mRef, filePath, info);
      Exception::MaybeThrow();
      return result;
    }

    // constructs the node based on a persisted JSON file
    bool loadFromFile(const char * filePath, PersistenceInfo * info = NULL, bool asReferenced = false)
    {
      bool result = FECS_DGGraph_loadFromFile(mRef, filePath, info, asReferenced);
      Exception::MaybeThrow();
      return result;
    }

    // reloads an already referenced graph from file
    bool reloadFromFile(PersistenceInfo * info = NULL)
    {
      bool result = FECS_DGGraph_reloadFromFile(mRef, info);
      Exception::MaybeThrow();
      return result;
    }

    // returns true if this graph is referenced from a file
    bool isReferenced()
    {
      bool result = FECS_DGGraph_isReferenced(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // returns the splice file path referenced by this graph
    const char * getReferencedFilePath()
    {
      const char * result = FECS_DGGraph_getReferencedFilePath(mRef);
      Exception::MaybeThrow();
      return result;
    }

    // marks a member to be persisted
    void setMemberPersistence(const char * name, bool persistence)
    {
      FECS_DGGraph_setMemberPersistence(mRef, name, persistence);
      Exception::MaybeThrow();
    }

  private:
    FECS_DGGraphRef mRef;
  };

  class SceneManagement
  {
  public:

    // // sets the callback for manipulation
    // static void setManipulationFunc(ManipulationFunc func)
    // {
    //   FECS_SceneManagement_setManipulationFunc(func);
    //   Exception::MaybeThrow();
    // }

    // returns true if there is anything to render
    static bool hasRenderableContent()
    {
      bool result = FECS_SceneManagement_hasRenderableContent();
      Exception::MaybeThrow();
      return result;
    }

    // draw all drawable ports
    // ensure to only call this with a valid
    // OpenGL context set, otherwise it might
    // cause instabilities
    static void drawOpenGL(FabricCore::RTVal & drawContext)
    {
      FECS_SceneManagement_drawOpenGL(drawContext);
      Exception::MaybeThrow();
    }

    // raycast against all raycastable objects
    // static bool raycast(FabricCore::RTVal & raycastContext, DGPort & port)
    // {
    //   FECS_DGPortRef portRef;
    //   bool result = FECS_SceneManagement_raycast(raycastContext, portRef);
    //   port = DGPort(portRef);
    //   Exception::MaybeThrow();
    //   return result;
    // }
  };

  class Scripting
  {

  public:

    // decodes a flat list of scripting arguments into a dictionary of 
    // argument values. this also assume one of the arguments (index 1 or 2)
    // to contain a json structure with additional values
    static FabricCore::Variant parseScriptingArguments(const char * action, const char * reference, const char * data, const char * auxiliary)
    {
      FabricCore::Variant result;
      FECS_Scripting_parseScriptingArguments(action, reference, data, auxiliary, result);
      Exception::MaybeThrow();
      return result;
    }

    // returns a bool argument of a given parsed dictionary
    static bool consumeBooleanArgument(FabricCore::Variant & argsDict, const char * name, bool defaultValue = false, bool optional = false)
    {
      bool result = FECS_Scripting_consumeBooleanArgument(argsDict, name, defaultValue, optional);
      Exception::MaybeThrow();
      return result;
    }

    // returns a int argument of a given parsed dictionary
    static int consumeIntegerArgument(FabricCore::Variant & argsDict, const char * name, int defaultValue = 0, bool optional = false)
    {
      int result = FECS_Scripting_consumeIntegerArgument(argsDict, name, defaultValue, optional);
      Exception::MaybeThrow();
      return result;
    }

    // returns a float argument of a given parsed dictionary
    static float consumeScalarArgument(FabricCore::Variant & argsDict, const char * name, float defaultValue = 0.0, bool optional = false)
    {
      float result = FECS_Scripting_consumeScalarArgument(argsDict, name, defaultValue, optional);
      Exception::MaybeThrow();
      return result;
    }

    // returns a string argument of a given parsed dictionary (string variant)
    static std::string consumeStringArgument(FabricCore::Variant & argsDict, const char * name, const char * defaultValue = "", bool optional = false)
    {
      char * resultPtr = FECS_Scripting_consumeStringArgument(argsDict, name, defaultValue, optional);
      Exception::MaybeThrow();
      if(resultPtr == NULL)
        return defaultValue;
      std::string result = resultPtr;
      FECS_Free(resultPtr);
      return result;
    }

    // returns a variant argument of a given parsed dictionary
    static FabricCore::Variant consumeVariantArgument(FabricCore::Variant & argsDict, const char * name, const FabricCore::Variant & defaultValue = FabricCore::Variant(), bool optional = false)
    {
      FabricCore::Variant result;
      FECS_Scripting_consumeVariantArgument(argsDict, name, defaultValue, optional, result);
      Exception::MaybeThrow();
      return result;
    }
  };
}

#endif // __cplusplus

#endif
