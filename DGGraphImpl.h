// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __FabricSpliceImpl_DGGRAPHIMPL_H__
#define __FabricSpliceImpl_DGGRAPHIMPL_H__

#include "DGPortImpl.h"
#include "SceneManagementImpl.h"
#include <FabricCore.h>

namespace FabricSpliceImpl
{
  class DGGraphImpl : public ObjectImpl
  {
    friend class SceneManagementImpl;

  public:

    typedef const char *(*GetOperatorSourceCodeFunc)(const char * graphName, const char * opName);

    struct PersistenceInfo
    {
      FabricCore::Variant hostAppName;
      FabricCore::Variant hostAppVersion;
      FabricCore::Variant filePath;
    };

    /*
      Constructors / Destructors
    */

    // create the FabricCore client
    static const FabricCore::Client * constructClient(bool guarded = true, FabricCore::ClientOptimizationType optType = FabricCore::ClientOptimizationType_Synchronous);

    static void setLicenseType(FabricCore::ClientLicenseType licenseType);

    // destroy the FabricCore client
    static bool destroyClient(bool force = false);

    // create the 
    static DGGraphImplPtr construct(const std::string & name, bool guarded = true, FabricCore::ClientOptimizationType optType = FabricCore::ClientOptimizationType_Synchronous);
    virtual ~DGGraphImpl();

    /// empties the content of the node
    void clear(std::string * errorOut = NULL);

    /// returns the client
    static const FabricCore::Client * getClient();

    /// retrieve the user pointer
    void * getUserPointer();

    /// store a user pointer
    void setUserPointer(void * data);

    /// sets the name and ensures name uniqueness
    virtual bool setName(const std::string & name);

    /// adds a search folder for extensions
    static bool addExtFolder(const std::string & folder, std::string * errorOut = NULL);

    /// set a callback to allow the splice persistence framework to gather
    /// the last unsaved code for a given KL operator. this code might still
    /// sit in the UI somewhere but hasn't been pushed to the DGGraph.
    static void setDCCOperatorSourceCodeCallback(GetOperatorSourceCodeFunc func);

    /*
      Metadata
    */

    /// retrieves the metadata from the DGGraph
    const std::string & getMetaData() { return mMetaData; }

    /// sets metadata on the DGGraph
    void setMetaData(const std::string & json) { mMetaData = json; }

    /*
      DG Graph management
    */
    
    /// create a new FabricCore::DGNode
    bool constructDGNode(const std::string & name = "", std::string * errorOut = NULL);
    
    /// returns true if a particular FabricCore::DGNode exists 
    bool hasDGNode(const std::string & name = "") const;
    
    /// removes a particular FabricCore::DGNode
    bool removeDGNode(const std::string & name = "", std::string * errorOut = NULL);

    /// returns the number of DGNodes
    unsigned int getDGNodeCount() const;

    /// returns a FabricCore::DGNode based on an index
    FabricCore::DGNode getDGNode(unsigned int index);
    
    /// returns a FabricCore::DGNode based on a name
    FabricCore::DGNode getDGNode(const std::string & name = "");

    /// returns all names of all contained FabricCore::DGNode
    char const * getDGNodeName(unsigned int index);
    
    /// returns all names of all contained FabricCore::DGNode
    stringVector getDGNodeNames() const;
    
    /// adds a member to a given FabricCore::DGNode based on a member name and a type (rt)
    bool addDGNodeMember(const std::string & name, const std::string & rt, FabricCore::Variant defaultValue = FabricCore::Variant(), const std::string & dgNode = "", const std::string & extension = "", bool autoInitObjects = true, std::string * errorOut = NULL);
    
    /// returns true if a specific member (on an optional specific FabricCore::DGNode) exists
    bool hasDGNodeMember(const std::string & name, const std::string & dgNode = "");

    /// returns the data type of a specific member (on an optional specific FabricCore::DGNode)
    std::string getDGNodeMemberDataType(const std::string & name, const std::string & dgNode = "");

    /// removes a member from a container FabricCore::DGNode
    bool removeDGNodeMember(const std::string & name, const std::string & dgNode = "", std::string * errorOut = NULL);

    /*
      DG operator management
    */

    /// returns KL source code for the parameter list for all available ports
    std::string generateKLOperatorParameterList(bool useLineBreaks = false);

    /// returns dummy source code for a KL operator
    std::string generateKLOperatorSourceCode(const std::string & name, const std::string & additionalBody = "", const std::string & additionalFunctions = "", const std::string & executeParallelMember = "");

    /// constructs a FabricCore::DGOperator based on a name and a kl source string (on an optional FabricCore::DGNode)
    bool constructKLOperator(const std::string & name, const std::string & sourceCode = "", const std::string & entry = "", const std::string & dgNode = "", const FabricCore::Variant & opPortMap = FabricCore::Variant::CreateDict(), std::string * errorOut = NULL);

    /// removes a FabricCore::DGOperator based on a name
    bool removeKLOperator(const std::string & name, const std::string & dgNode = "", std::string * errorOut = NULL);

    /// returns true if this node contains a given operator
    bool hasKLOperator(const std::string & name,const std::string & dgNodeName = "", std::string * errorOut = NULL);

    /// gets the entry function name of a specific FabricCore::DGOperator
    char const * getKLOperatorEntry(const std::string & name, std::string * errorOut = NULL);

    /// sets the entry function name of a specific FabricCore::DGOperator
    bool setKLOperatorEntry(const std::string & name, const std::string & entry, std::string * errorOut = NULL);

    // moves the FabricCore::DGOperator on the stack to a given index
    bool setKLOperatorIndex(const std::string & name, unsigned int index, std::string * errorOut = NULL);

    /// gets the source code of a specific FabricCore::DGOperator
    char const * getKLOperatorSourceCode(const std::string & name, std::string * errorOut = NULL);

    /// sets the source code of a specific FabricCore::DGOperator
    bool setKLOperatorSourceCode(const std::string & name, const std::string & sourceCode, const std::string & entry, std::string * errorOut = NULL);

    /// loads the source code of a specific FabricCore::DGOperator from file
    void loadKLOperatorSourceCode(const std::string & name, const std::string & filePath, std::string * errorOut = NULL);

    /// saves the source code of a specific FabricCore::DGOperator to file
    void saveKLOperatorSourceCode(const std::string & name, const std::string & filePath, std::string * errorOut = NULL);

    /// returns true if the KL operator is using a file
    bool isKLOperatorFileBased(const std::string & name, std::string * errorOut = NULL);

    /// gets the filepath of a specific FabricCore::DGOperator
    char const * getKLOperatorFilePath(const std::string & name, std::string * errorOut = NULL);

    /// loads the content of the file and sets the code
    void setKLOperatorFilePath(const std::string & name, const std::string & filePath, const std::string & entry = "", std::string * errorOut = NULL);

    /// returns the number of operators
    unsigned int getKLOperatorCount(const std::string & dgNodeName = "", std::string * errorOut = NULL);

    /// returns the names of the operators
    char const * getKLOperatorName(unsigned int index, const std::string & dgNodeName = "", std::string * errorOut = NULL);

    /// returns the names of the operators
    stringVector getKLOperatorNames(const std::string & dgNodeName = "", std::string * errorOut = NULL);

    /// returns the global number of the operators
    static unsigned int getGlobalKLOperatorCount(std::string * errorOut = NULL);

    /// returns a specific name of all global operators
    static char const * getGlobalKLOperatorName(unsigned int index, std::string * errorOut = NULL);

    /// returns the names of all global operators
    static stringVector getGlobalKLOperatorNames(std::string * errorOut = NULL);

    /// loads a KL extension
    static bool loadExtension(const std::string & extName, std::string * errorOut = NULL);

    /*
      Dependency graph evaluation
    */

    /// checks all FabricCore::DGNodes and FabricCore::Operators for errors, return false if any errors found
    static bool checkErrors(std::string * errorOut = NULL);

    /// set whether to evaluate using a shared lock
    void setEvaluateShared( bool evaluateShared )
        { mEvaluateShared = evaluateShared; }

    /// evaluates a provided FabricCore::DGNode
    bool evaluate(
        FabricCore::DGNode dgNode,
        std::string * errorOut = NULL
        );

    /// evaluates a FabricCore::DGNode based on its name
    bool evaluate(
        const std::string & name = "",
        std::string * errorOut = NULL
        );

    /// clears the evaluation state
    bool clearEvaluate(std::string * errorOut = NULL);

    /// returns if this graph is using the eval context
    bool usesEvalContext();

    /// returns the graph's evaluation context object
    FabricCore::RTVal getEvalContext();

    /*
      Port management
    */

    /// adds a new Port provided a name, the FabricCore::DGNode member, a mode and an optional FabricCore::DGNode name
    /// you need to pass in thisGraph as a shared pointer to avoid cycles in reference counting.
    DGPortImplPtr addDGPort(DGGraphImplPtr thisGraph, const std::string & name, const std::string & member, DGPortImpl::Mode mode, const std::string & dgNode = "", bool autoInitObjects = true, std::string * errorOut = NULL);

    /// removes an existing Port by name
    bool removeDGPort(const std::string & name, std::string * errorOut = NULL);

    /// returns a specific Port by name
    DGPortImplPtr getDGPort(const std::string & name);

    /// returns the name of a port given an index
    char const * getDGPortName(unsigned int index);

    /// returns the names of all current Ports
    stringVector getDGPortNames();

    /// returns a specific Port by index
    DGPortImplPtr getDGPort(unsigned int index);

    /// returns the number of ports
    unsigned int getDGPortCount();

    /*
      Subgraph management
    */

    /// returns true if a given DG node is dependent on another one
    bool hasDGNodeDependency(const std::string & dgNode, const std::string & dependency, std::string * errorOut = NULL);

    /// depends one DGNode on another one
    bool setDGNodeDependency(const std::string & dgNode, const std::string & dependency, std::string * errorOut = NULL);

    /// depends one DGNode on another one
    bool removeDGNodeDependency(const std::string & dgNode, const std::string & dependency, std::string * errorOut = NULL);

    /*
      Persistence
    */

    /// returns a variant encoding the information about all ports
    void getDGPortInfo(FabricCore::Variant & portInfo, FabricCore::RTVal persistenceContextRT = FabricCore::RTVal(), bool forPersistence = false);

    /// returns a dictionary of the persistence data of a node
    FabricCore::Variant getPersistenceDataDict(const PersistenceInfo * info = NULL);

    /// returns JSON string encoding the persistence data of a node
    std::string getPersistenceDataJSON(const PersistenceInfo * info = NULL);

    /// constructs the node based on a variant dict
    /// you need to pass in thisGraph as a shared pointer to avoid cycles in reference counting.
    bool setFromPersistenceDataDict(DGGraphImplPtr thisGraph, const FabricCore::Variant & dict, PersistenceInfo * info = NULL, const char * baseFilePath = NULL, std::string * errorOut = NULL);

    /// constructs the node based on a JSON string
    /// you need to pass in thisGraph as a shared pointer to avoid cycles in reference counting.
    bool setFromPersistenceDataJSON(DGGraphImplPtr thisGraph, const std::string & json, PersistenceInfo * info = NULL, const char * baseFilePath = NULL, std::string * errorOut = NULL);

    /// persists the node description into a JSON file
    bool saveToFile(const std::string & filePath, const PersistenceInfo * info = NULL, std::string * errorOut = NULL);

    /// constructs the node based on a persisted JSON file
    /// you need to pass in thisGraph as a shared pointer to avoid cycles in reference counting.
    bool loadFromFile(DGGraphImplPtr thisGraph, const std::string & filePath, PersistenceInfo * info = NULL, bool asReferenced = false, std::string * errorOut = NULL);

    /// reloads an already referenced graph from file
    bool reloadFromFile(DGGraphImplPtr thisGraph, PersistenceInfo * info = NULL, std::string * errorOut = NULL);

    /// returns true if this graph is referenced from a file
    bool isReferenced();

    /// returns the splice reference file path
    const char * getReferencedFilePath();

    /// request an evaluation on idle
    bool requireEvaluate();

    /// complex data types and arrays are not persisted, use this to override the default behaviour 
    void setMemberPersistence(const std::string &name, bool persistence);

  private:

    // disable constructor for public use
    DGGraphImpl(const std::string & name, bool guarded, FabricCore::ClientOptimizationType optType);

    // disable copy constructor
    DGGraphImpl(const DGGraphImpl& that) {}

    // fire an evaluation on idle
    static void requireDGCheck() { sDGCheckRequired = true; }

    /// marks a given KL operator as invalid
    static bool invalidateKLOperator(const std::string & opName, std::string * errorOut = NULL);

    /// updates a specific kl operator and marks it as valid (or not)
    static bool validateKLOperator(const std::string & opName, bool logValidation = false, std::string * errorOut = NULL);

    /// updates all of the kl operators and marks them as valid (or not)
    static bool validateAllKLOperators(std::string * errorOut = NULL);

    /// checks a name for validness
    static bool isValidName(const std::string & name, const std::string & context, std::string * errorOut = NULL);

    /// gets the source code of a specific FabricCore::DGOperator
    static char const * getKLOperatorSourceCodeByRealOpName(const std::string & realOpName, std::string * errorOut = NULL);

    struct DGNodeData
    {
      FabricCore::DGNode node;
      std::vector<FabricCore::Variant> opPortMaps;
    };

    struct DGBindingData
    {
      size_t index;
      std::string opName;
      bool valid;
      std::string dgNode;
      std::vector<std::string> portName;   
    };

    struct DGOperatorParamInfo
    {
      std::string mode;
      std::string name;
      std::string dataType;
      std::string memberType;
      bool isArray;
      bool isSliced;
      std::string binding;
    }; 

    struct DGOperatorData
    {
      FabricCore::DGOperator op;
      std::vector<DGOperatorParamInfo> params;
      size_t uses;
      std::string entry;
      std::string klCode;

      DGOperatorData(FabricCore::DGOperator inOp, std::string inEntry, std::string inCode)
      {
        op = inOp;
        uses = 1;
        entry = inEntry;
        klCode = inCode;
      }
    };

    typedef std::map<std::string, DGNodeData> DGNodeMap;
    typedef DGNodeMap::iterator DGNodeIt;
    typedef DGNodeMap::const_iterator DGNodeConstIt;
    typedef std::pair<std::string, DGNodeData> DGNodePair;
    
    typedef std::map<std::string, DGOperatorData> DGOperatorMap;
    typedef DGOperatorMap::iterator DGOperatorIt;
    typedef DGOperatorMap::const_iterator DGOperatorConstIt;
    typedef std::pair<std::string, DGOperatorData> DGOperatorPair;
    typedef std::map<std::string, size_t> DGOperatorSuffixMap;
    typedef DGOperatorSuffixMap::iterator DGOperatorSuffixIt;
    typedef DGOperatorSuffixMap::const_iterator DGOperatorSuffixConstIt;
    typedef std::pair<std::string, size_t> DGOperatorSuffixPair;

    static stringMap getDGGraphNamesMap();
    static stringVector findRequireStatements(const std::string & klCode, std::string * errorOut = NULL);
    static bool checkErrorVariant(const FabricCore::Variant * variant, std::string * errorOut);
    static DGPortImplPtr getDGPortFromKey(const std::string & key);
    char const * getRealDGOperatorName(const char * name) const;
    char const * getPrettyDGOperatorName(const char * name) const;

    // instance members
    stringMap mDGOperatorNameMap;
    DGNodeMap mDGNodes;
    DGPortMap mDGPorts;
    std::string mDGNodeDefaultName;
    bool mRequiresEval;
    bool mIsPersisting;
    bool mIsClearing;
    bool mUsesEvalContext;
    std::map<std::string, int> mMemberPersistenceOverrides;
    void * mUserPointer;
    std::vector<DGBindingData> mBindings;
    stringMap mLoadedExtensions;
    std::string mMetaData;
    FabricCore::RTVal mEvalContext;
    stringMap mKLOperatorFileNames;
    bool mIsReferenced;
    std::string mFilePath;
    std::string mOriginalName;
    bool mEvaluateShared;

    // static members
    static DGOperatorSuffixMap sDGOperatorSuffix;
    static DGOperatorMap sDGOperators;
    static bool sDGCheckRequired;
    static unsigned int sInstanceCount;
    static FabricCore::Client * sClient;
    static FabricCore::RTVal sDrawingScope;
    static bool sClientOwnedByGraph;
    static stringMap sClientRTs;
    static std::vector<DGGraphImpl*> sAllDGGraphs;
    static stringVector sRTFolders;
    static stringVector sExtFolders;
    static GetOperatorSourceCodeFunc sGetOperatorSourceCodeFunc;
    static FabricCore::ClientLicenseType sCoreLicenseType;

    // utilities
    bool memberPersistence(const std::string &name, const std::string &type, bool * requiresStorage = NULL);
    static std::string resolveRelativePath(const std::string & baseFile, const std::string text);
    static std::string resolveEnvironmentVariables(const std::string text);
    FILE* findFileInSearchPath(std::string& resolvedFilePath);
  };
};

#endif
