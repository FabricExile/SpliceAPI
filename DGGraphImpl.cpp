// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "DGGraphImpl.h"
#include "SceneManagementImpl.h"
#include "KLParserImpl.h"

#include <FTL/FS.h>
#include <FabricServices/Persistence/RTValToJSONEncoder.hpp>
#include <FabricServices/Persistence/RTValFromJSONDecoder.hpp>

#include <fstream>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>

#include <boost/version.hpp>
#include <boost/algorithm/string.hpp>
#include "FabricSplice.h"

using namespace FabricSpliceImpl;

bool DGGraphImpl::sDGCheckRequired = false;
unsigned int DGGraphImpl::sInstanceCount = 0;
FabricCore::Client * DGGraphImpl::sClient = NULL;
FabricCore::RTVal DGGraphImpl::sDrawingScope;
bool DGGraphImpl::sClientOwnedByGraph = false;
stringMap DGGraphImpl::sClientRTs;
std::vector<DGGraphImpl*> DGGraphImpl::sAllDGGraphs;
DGGraphImpl::DGOperatorMap DGGraphImpl::sDGOperators;
DGGraphImpl::DGOperatorSuffixMap DGGraphImpl::sDGOperatorSuffix;
FabricServices::Persistence::RTValToJSONEncoder sRTValEncoder;
FabricServices::Persistence::RTValFromJSONDecoder sRTValDecoder;
FabricCore::ClientLicenseType DGGraphImpl::sCoreLicenseType = UINT8_MAX;

stringVector DGGraphImpl::sExtFolders;
DGGraphImpl::GetOperatorSourceCodeFunc DGGraphImpl::sGetOperatorSourceCodeFunc = NULL;

void klReportFunc(
  void *userdata,
  FabricCore::ReportSource source,
  FabricCore::ReportLevel level,
  char const *reportData,
  uint32_t reportLength
  )
{
  std::string message(reportData);
  LoggingFunc reportFunc = LoggingImpl::getKLReportFunc();
  if ( reportFunc == NULL )
    printf("%s\n", message.c_str());
  else
    (*reportFunc)(message.c_str(), message.length());
}

void klStatusFunc(void *userdata, char const *destData, uint32_t destLength,
  char const *payloadData, uint32_t payloadLength)
{
  std::string topic(destData, destLength);
  std::string message(payloadData, payloadLength);
  StatusFunc statusFunc = LoggingImpl::getKLStatusFunc();
  if(statusFunc == NULL)
    printf("%s: %s\n", topic.c_str(), message.c_str());
  else
    (*statusFunc)(topic.c_str(), topic.length(), message.c_str(), message.length());
}

void klSlowOperationFunc(
  void *userdata,
  char const *descCStr, uint32_t descLength
  )
{
  SlowOperationFunc slowOperationFunc = LoggingImpl::getSlowOperationFunc();
  if ( slowOperationFunc )
    (*slowOperationFunc)( descCStr, descLength );
}

const FabricCore::Client * DGGraphImpl::constructClient(bool guarded, FabricCore::ClientOptimizationType optType)
{
  if(sClient == NULL)
  {
    sClientOwnedByGraph = false;

    FabricCore::Client::CreateOptions options;
    memset(&options, 0, sizeof(options));
    options.guarded = 1;
    if(!guarded)
      options.guarded = 0;
    options.traceOperators = 0;
    options.optimizationType = optType;

    if (sCoreLicenseType == UINT8_MAX)
      throw FabricCore::Exception(
        "Cannot create Core Client, the License Type is not set" );
    options.licenseType = sCoreLicenseType;

    options.rtValToJSONEncoder = &sRTValEncoder;
    options.rtValFromJSONDecoder = &sRTValDecoder;

    std::vector<const char *> extsPaths(sExtFolders.size());
    if(sExtFolders.size() > 0)
    {
      for(size_t i=0;i<extsPaths.size();i++)
        extsPaths[i] = sExtFolders[i].c_str();
      options.numExtPaths = extsPaths.size();
      options.extPaths = &extsPaths[0];
    }
    else
    {
      options.numExtPaths = 0;
      options.extPaths = 0;
    }
    options.numExtsToLoad = 0;
    options.slowOperationCallback = &klSlowOperationFunc;

    sClient = new FabricCore::Client();
    *sClient = FabricCore::Client::GetSingleton(&klReportFunc, 0, &options);

    // setup KL reporting
    sClient->setReportCallback(&klReportFunc, NULL);
    sClient->setStatusCallback(&klStatusFunc, NULL);

    // define all of the core types    
    sClientRTs.insert(stringPair("Boolean", ""));
    sClientRTs.insert(stringPair("Byte", ""));
    sClientRTs.insert(stringPair("UInt8", ""));
    sClientRTs.insert(stringPair("UInt16", ""));
    sClientRTs.insert(stringPair("UInt32", ""));
    sClientRTs.insert(stringPair("UInt64", ""));
    sClientRTs.insert(stringPair("SInt8", ""));
    sClientRTs.insert(stringPair("SInt16", ""));
    sClientRTs.insert(stringPair("SInt32", ""));
    sClientRTs.insert(stringPair("SInt64", ""));
    sClientRTs.insert(stringPair("Integer", ""));
    sClientRTs.insert(stringPair("Size", ""));
    sClientRTs.insert(stringPair("Index", ""));
    sClientRTs.insert(stringPair("Float32", ""));
    sClientRTs.insert(stringPair("Float64", ""));
    sClientRTs.insert(stringPair("Scalar", ""));
    sClientRTs.insert(stringPair("String", ""));

    // define KL parsers for arrays and dicts
    std::string klCode;
    klCode += "// returns the size of the array\n";
    klCode += "function Size array.size() {};\n";
    klCode += "// sets a new size of the array.\n";
    klCode += "// the new elements are cloned from the last element.\n";
    klCode += "function array.resize(Size newSize) {};\n";
    KLParserImpl::getParser("array", "array", klCode.c_str());
    klCode.clear();
    klCode += "// returns true if the dict contains a certain key\n";
    klCode += "function ValueType dict.has(KeyType key) {};\n";
    KLParserImpl::getParser("dict", "dict", klCode.c_str());
    klCode.clear();
    klCode += "// returns the length of the String\n";
    klCode += "function Size String.length() {};\n";
    KLParserImpl::getParser("String", "String", klCode.c_str());

    // load the extension we will always need
    loadExtension("Math");
    loadExtension("Geometry");
    loadExtension("Singletons");
    loadExtension("InlineDrawing");
#ifdef FABRIC_SCENEHUB
    loadExtension("SceneHub");
#endif
    loadExtension("Parameters");
    loadExtension("Manipulation");
    loadExtension("FileIO");
    loadExtension("Util");
    loadExtension("FabricInterfaces");

    // define the singletons scope
    sDrawingScope = FabricCore::RTVal::Create(*sClient, "InlineDrawingScope", 0, 0);
  }
  return (const FabricCore::Client *)sClient;
}

void DGGraphImpl::setLicenseType(FabricCore::ClientLicenseType licenseType)
{
  sCoreLicenseType = licenseType;
}

bool DGGraphImpl::destroyClient(bool force)
{
  if(sInstanceCount == 0 || force)
  {
    KLParserImpl::resetAll();
    sClientRTs.clear();
    // SceneManagementImpl::reset();

    if(sClient != NULL)
    {
      sDrawingScope.invalidate();
      sClient->invalidate();
      delete(sClient);
      sClient = NULL;
    }
  }
  return false;
}

DGGraphImpl::DGGraphImpl(
  const std::string & name,
  bool guarded, 
  FabricCore::ClientOptimizationType optType
) {

  setName(name);
  mOriginalName = getName();
  mDGNodeDefaultName = "DGNode";
  mRequiresEval = false;
  mIsPersisting = false;
  mIsClearing = true;
  mUsesEvalContext = false;
  mUserPointer = NULL;
  mIsReferenced = false;

  static bool haveDefaultEvaluateShared = false;
  static bool defaultEvaluateShared;
  if ( !haveDefaultEvaluateShared )
  {
    char const *envvar = ::getenv( "FABRIC_SPLICE_PARALLEL_DEFAULT" );
    defaultEvaluateShared = envvar && atoi( envvar ) > 0;
    haveDefaultEvaluateShared = true;
  }
  mEvaluateShared = defaultEvaluateShared;

  try
  {
    if(sInstanceCount == 0 && sClient == NULL)
    {
      constructClient(guarded, optType);
      sClientOwnedByGraph = true;
    }

    sAllDGGraphs.push_back(this);
    sInstanceCount++;
  }
  catch(FabricCore::Exception e)
  {
    LoggingImpl::logError(e.getDesc_cstr());
    throw(e);
  }

  // construct this graph's evaluation context
  mEvalContext = FabricCore::RTVal::Create(*sClient, "EvalContext", 0, 0);

  LoggingImpl::log("DGGraph '"+getName()+"' created.");
}

DGGraphImplPtr DGGraphImpl::construct(const std::string & name, bool guarded, FabricCore::ClientOptimizationType optType)
{
  return DGGraphImplPtr(new DGGraphImpl(name, guarded, optType));
}

DGGraphImpl::~DGGraphImpl()
{
  clear();
  LoggingImpl::log("DGGraph '"+getName()+"' destroyed.");

  sInstanceCount--;

  // remove the graph from the static list
  for(size_t i=0;i<sAllDGGraphs.size();i++)
  {
    if(sAllDGGraphs[i] == this)
    {
      sAllDGGraphs.erase(sAllDGGraphs.begin() + i);
      break;
    }
  }

  // if we are the last one, close the client
  if(sInstanceCount == 0)
  {
    sDGOperators.clear();
    sDGOperatorSuffix.clear();
  }

  if(sClientOwnedByGraph)
    destroyClient();

  requireDGCheck();
}

void DGGraphImpl::clear(std::string * errorOut)
{
  mIsClearing = true;

  mLoadedExtensions.clear();

  // remove all operators
  stringVector opsToRemove;
  for(stringIt it = mDGOperatorNameMap.begin(); it != mDGOperatorNameMap.end(); it++)
    opsToRemove.push_back(it->second);
  for(size_t i=0;i<opsToRemove.size();i++)
    removeKLOperator(opsToRemove[i]);

  // remove all ports
  stringVector portsToRemove;
  for(DGPortIt portIt=mDGPorts.begin();portIt!=mDGPorts.end();portIt++)
    portsToRemove.push_back(portIt->first);
  for(size_t i=0;i<portsToRemove.size();i++)
    removeDGPort(portsToRemove[i]);

  mDGPorts.clear();
  mMemberPersistenceOverrides.clear();

  for(DGNodeIt nodeIt = mDGNodes.begin(); nodeIt != mDGNodes.end(); nodeIt++)
  {
    nodeIt->second.node.destroy();
    LoggingImpl::log("DGGraph '"+getName()+"' removed DGNode '"+nodeIt->first+"'.");
  }

  mDGNodes.clear();
  mIsClearing = false;
  mDGNodeDefaultName = "DGNode";
  requireDGCheck();
  requireEvaluate();
}

const FabricCore::Client * DGGraphImpl::getClient()
{
  return sClient;
}

void * DGGraphImpl::getUserPointer()
{
  return mUserPointer;
}

void DGGraphImpl::setUserPointer(void * data)
{
  mUserPointer = data;
}


bool DGGraphImpl::setName(const std::string & name)
{
  // start with an empty name
  // this is important for the name map
  // to generate a unique name
  std::string uniqueName = "";
  ObjectImpl::setName(uniqueName);
  uniqueName = StringUtilityImpl::getUniqueString(name, getDGGraphNamesMap());
  ObjectImpl::setName(uniqueName);
  return getName() == name;
}

bool DGGraphImpl::addExtFolder(const std::string & folder, std::string * errorOut)
{
  for(size_t i=0;i<sExtFolders.size();i++)
  {
    if(sExtFolders[i] == folder)
      return false;
  }
  sExtFolders.push_back(folder);

  return true;
}

void DGGraphImpl::setDCCOperatorSourceCodeCallback(DGGraphImpl::GetOperatorSourceCodeFunc func)
{
  sGetOperatorSourceCodeFunc = func;
}

stringMap DGGraphImpl::getDGGraphNamesMap()
{
  stringMap result;
  for(size_t i=0;i<sAllDGGraphs.size();i++)
  {
    if(result.find(sAllDGGraphs[i]->getName()) == result.end())
      result.insert(stringPair(sAllDGGraphs[i]->getName(), ""));
    if(result.find(sAllDGGraphs[i]->mOriginalName) == result.end())
      result.insert(stringPair(sAllDGGraphs[i]->mOriginalName, ""));
  }
  return result;
}

bool DGGraphImpl::loadExtension(const std::string & extName, std::string * errorOut)
{
  std::string name = extName;
  if(name.find('[') != std::string::npos)
    name = name.substr(0, name.find('['));
  
  // check if the extension is loaded already
  if(sClientRTs.find(name) != sClientRTs.end())
    return true;
  if(sClientRTs.find("RT" + name) != sClientRTs.end())
    return true;

  if(sClient == NULL)
    return LoggingImpl::reportError("No FabricCore Client created yet.", errorOut);

  // check if this is a type which is part of another extension
  try
  {
    const char * realExtName = FabricCore::GetRegisteredTypeExtName(*sClient, name.c_str());
    if(realExtName != NULL)
    {
      if(realExtName != name && realExtName != extName)
      {
        if(loadExtension(realExtName, errorOut))
        {
          sClientRTs.insert(stringPair(name, name));
          return true;
        }
      }
    }
  }
  catch(FabricCore::Exception e)
  {
  }

  // finally load the extension from the core
  try
  {
    sClient->loadExtension(name.c_str(), "", false);
  }
  catch(FabricCore::Exception e)
  {
    LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
    return false;
  }

  sClientRTs.insert(stringPair(name, name));

  // if we have loaded this type, let's find the corresponding 
  // code files and parse them
  stringVector paths = sExtFolders;
  char * pathChar = getenv("FABRIC_EXTS_PATH");
  if(pathChar == NULL && paths.size() == 0)
    return LoggingImpl::reportError("Cannot load extension '"+name+"', FABRIC_EXTS_PATH not specified..", errorOut);
  else if(pathChar != NULL)
  {
    std::string path = pathChar;
    if(path.length() == 0 && paths.size() == 0)
      return LoggingImpl::reportError("Cannot load extension '"+name+"', FABRIC_EXTS_PATH not specified.", errorOut);
    else if(path.length() > 0)
    {
#ifdef _WIN32
      stringVector newPaths = StringUtilityImpl::splitString(path, ';');
#else 
      stringVector newPaths = StringUtilityImpl::splitString(path, ':');
#endif
      paths.insert( paths.end(), newPaths.begin(), newPaths.end() );  
    }
  }

  std::string fpmPath;
  FILE * fpmFile = NULL;
  for(size_t i=0;i<paths.size();i++)
  {
    if (paths[i].empty())
      continue;

#ifdef _WIN32
    paths[i] = StringUtilityImpl::replaceString(paths[i], '/', '\\');
    if(!StringUtilityImpl::endsWith(paths[i], "\\"))
      paths[i] += "\\";
#else
    paths[i] = StringUtilityImpl::replaceString(paths[i], '\\', '/');
    if(!StringUtilityImpl::endsWith(paths[i], "/"))
      paths[i] += "/";
#endif
    fpmPath = paths[i];
    std::string fpmFilePath = paths[i] + name;
    if(!StringUtilityImpl::endsWith(fpmFilePath,".fpm.json"))
      fpmFilePath += ".fpm.json";

    fpmFile = fopen(fpmFilePath.c_str(), "rb");
    if(fpmFile)
      break;

    std::vector<std::string> additionalPaths;

    // add subfolders
    try
    {
      FTL::StrRef path = paths[i];
      if( FTL::FSExists(path)) {
        std::vector<std::string> entries;
        FTL::FSDirAppendEntries(path, entries);
        for ( size_t i=0; i<entries.size(); ++i ) {
          FTL::StrRef entry = entries[i];
          std::string entryPath = FTL::PathJoin( path, entry );
          if(!FTL::FSIsDir(entryPath))
            continue;
          additionalPaths.push_back(entryPath);
        }
      }
    }
    catch ( std::exception e )
    {
      LoggingImpl::reportError(std::string("Unable to read Ext sub-folder: '") +
          paths[i] + std::string("'"), errorOut);
    }

    paths.insert(
      paths.end(), additionalPaths.begin(), additionalPaths.end()
      );
  }

  std::vector<std::string> extKlFiles;
  if(fpmFile != NULL)
  {
    fseek( fpmFile, 0, SEEK_END );
    int fileSize = ftell( fpmFile );
    rewind( fpmFile );

    char * jsonBuffer = (char*) malloc(fileSize + 1);
    jsonBuffer[fileSize] = '\0';

    fread(jsonBuffer, fileSize, 1, fpmFile);
    fclose(fpmFile);

    std::string json = jsonBuffer;
    free(jsonBuffer);

    FabricCore::Variant jsonDict;
    try
    {
      jsonDict = FabricCore::Variant::CreateFromJSON(json.c_str());
    }
    catch(FabricCore::Exception e)
    {
    }
    if(!jsonDict.isNull())
    {
      const FabricCore::Variant * extCodeVar = jsonDict.getDictValue("code");
      if(extCodeVar)
      {
        if(extCodeVar->isArray())
        {
          for(uint32_t i=0;i<extCodeVar->getArraySize();i++)
          {
            const FabricCore::Variant * extCodeElementVar = extCodeVar->getArrayElement(i);
            if(!extCodeElementVar)
              continue;
            if(!extCodeElementVar->isString())
              continue;
            extKlFiles.push_back(extCodeElementVar->getStringData());
          }
        }
        else if(extCodeVar->isString())
          extKlFiles.push_back(extCodeVar->getStringData());
      }
    }
  }

  for(size_t i=0;i<extKlFiles.size();i++)
  {
    std::string extKlFilePath = fpmPath + extKlFiles[i];
    if(!StringUtilityImpl::endsWith(extKlFilePath,".kl"))
      extKlFilePath += ".kl";

    FILE * extKlFile = fopen(extKlFilePath.c_str(), "rb");
    if(extKlFile != NULL)
    {
      fseek( extKlFile, 0, SEEK_END );
      int fileSize = ftell( extKlFile );
      rewind( extKlFile );

      char * klCodeBuffer = (char*) malloc(fileSize + 1);
      klCodeBuffer[fileSize] = '\0';

      fread(klCodeBuffer, fileSize, 1, extKlFile);
      fclose(extKlFile);

      std::string extKlFileCode = klCodeBuffer;
      free(klCodeBuffer);

      KLParserImpl::getParser(name.c_str(), extKlFiles[i].c_str(), extKlFileCode.c_str());
    }
  }
  return true;
}

bool DGGraphImpl::constructDGNode(const std::string & name, std::string * errorOut)
{
  if(!isValidName(name, "DGNode"))
    return false;

  std::string dgNodeName = name;
  if(dgNodeName.length() == 0)
    dgNodeName = mDGNodeDefaultName;
  else if(mDGNodes.size() == 0)
    mDGNodeDefaultName = dgNodeName;

  // todo: this disabled all of the subgraph
  // functionality - this might have to go away soon
  if(dgNodeName != mDGNodeDefaultName)
  {
    std::string message = "Subgraphs are disabled in this release of Splice. \nThe next iteration of Splice will be based on the Fabric Dataflow Graph, \nwhich will provide Subgraph functionality.\nPlease contact Fabric Engine for more information.";
    return LoggingImpl::reportError(message, errorOut);
  }
  
  DGNodeIt it = mDGNodes.find(dgNodeName);
  if(it != mDGNodes.end())
  {
    std::string message = "DGNode '"+name+"' already exists.";
    return LoggingImpl::reportError(message, errorOut);
  }

  std::string fullDGNodeName = getName() + "_" + dgNodeName;
  try
  {
    DGNodeData data;
    data.node = FabricCore::DGNode(*sClient, fullDGNodeName.c_str());
    mDGNodes.insert(DGNodePair(dgNodeName, data));

    data.node.addMember_Variant("context", "EvalContext", FabricCore::Variant());
    data.node.setMemberSliceValue("context", 0, mEvalContext);
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  requireDGCheck();
  requireEvaluate();

  LoggingImpl::log("DGGraph '"+getName()+"' constructed new DGNode '"+dgNodeName+"'.");

  return true;
}

bool DGGraphImpl::hasDGNode(const std::string & name) const
{
  std::string dgNodeName = name;
  if(dgNodeName.length() == 0)
    dgNodeName = mDGNodeDefaultName;
  return mDGNodes.find(dgNodeName) != mDGNodes.end();
}

bool DGGraphImpl::removeDGNode(const std::string & name, std::string * errorOut)
{
  std::string dgNodeName = name;
  if(dgNodeName.length() == 0)
    dgNodeName = mDGNodeDefaultName;
  DGNodeIt it = mDGNodes.find(dgNodeName);
  if(it == mDGNodes.end())
  {
    std::string message = "DGNode '"+name+"' does not exist.";
    return LoggingImpl::reportError(message, errorOut);
  }

  // remove all port on this dgnode
  stringVector portsToRemove;
  for(DGPortIt portIt=mDGPorts.begin();portIt!=mDGPorts.end();portIt++)
  {
    if(portIt->second->getDGNodeName() == dgNodeName)
      portsToRemove.push_back(it->first);
  }
  for(size_t i=0;i<portsToRemove.size();i++)
    removeDGPort(portsToRemove[i]);

  FabricCore::DGNode dgNode = it->second.node;
  mDGNodes.erase(it);
  dgNode.destroy();
  requireDGCheck();
  requireEvaluate();
  return true;
}

unsigned int DGGraphImpl::getDGNodeCount() const
{
  return (unsigned int)mDGNodes.size();
}


FabricCore::DGNode DGGraphImpl::getDGNode(unsigned int index)
{
  if(index >= getDGNodeCount())
    return FabricCore::DGNode();
  DGNodeIt it = mDGNodes.begin();
  std::advance(it, index);
  return it->second.node;
}

FabricCore::DGNode DGGraphImpl::getDGNode(const std::string & name)
{
  std::string dgNodeName = name;
  if(dgNodeName.length() == 0)
    dgNodeName = mDGNodeDefaultName;
  DGNodeIt it = mDGNodes.find(dgNodeName);
  if(it == mDGNodes.end())
    return FabricCore::DGNode();
  return it->second.node;
}

char const * DGGraphImpl::getDGNodeName(unsigned int index)
{
  if(index >= getDGNodeCount())
    return "";
  DGNodeIt it = mDGNodes.begin();
  std::advance(it, index);
  return it->first.c_str();
}

bool DGGraphImpl::evaluate(
  FabricCore::DGNode dgNode,
  std::string * errorOut
  )
{
  if(!mRequiresEval || mIsPersisting)
    return true;

  if(!dgNode.isValid())
    return LoggingImpl::reportError("No valid DGNode provided.", errorOut);

  if(!checkErrors(errorOut))
  {
    SceneManagementImpl::setErrorStatus(true);
    return false;
  }
  SceneManagementImpl::setErrorStatus(false);

  try
  {
    dgNode.evaluate_lockType(
      mEvaluateShared?
        FabricCore::LockType_Shared:
        FabricCore::LockType_Exclusive
        );
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  mRequiresEval = false;
  return true;
}

bool DGGraphImpl::evaluate(
  const std::string & name,
  std::string * errorOut
  )
{
  // todo: for subgraphs here we need to probably evaluate the leaf 
  // output nodes
  FabricCore::DGNode dgNode = getDGNode(name);
  if(!dgNode.isValid())
    return LoggingImpl::reportError("DGNode '"+name+"' does not exist.", errorOut);
  return evaluate(dgNode, errorOut);
}

bool DGGraphImpl::clearEvaluate(std::string * errorOut)
{
  if(!mRequiresEval)
    return false;
  mRequiresEval = false;
  return true;
}

bool DGGraphImpl::usesEvalContext()
{
  return mUsesEvalContext;
}

FabricCore::RTVal DGGraphImpl::getEvalContext()
{
  return mEvalContext;
}

stringVector DGGraphImpl::getDGNodeNames() const
{
  stringVector result;
  for(DGNodeConstIt it = mDGNodes.begin(); it != mDGNodes.end(); it++)
    result.push_back(it->first);
  return result;
}

bool DGGraphImpl::addDGNodeMember(
  const std::string & name, 
  const std::string & rt, 
  FabricCore::Variant defaultValue,
  const std::string & dgNode, 
  const std::string & extension,
  bool autoInitObjects, 
  std::string * errorOut
) {
  FabricCore::DGNode node = getDGNode(dgNode);
  if(!node.isValid())
  {
    std::string message = "DGNode '"+dgNode+"' does not exist.";
    return LoggingImpl::reportError(message, errorOut);
  }

  if(extension.size() > 0)
  {
    if(mLoadedExtensions.find(extension) == mLoadedExtensions.end())
    {
      if(!loadExtension(extension, errorOut)) {
        std::string message = "Extension '"+extension+"' cannot be found.";
        return LoggingImpl::reportError(message, errorOut);
      }
      mLoadedExtensions.insert(stringPair(extension, extension));
    }
  }

  FabricCore::Variant members = node.getMembers_Variant();

  for(FabricCore::Variant::DictIter keyIter(members); !keyIter.isDone(); keyIter.next())
  {
    std::string key = keyIter.getKey()->getStringData();
    if(key == name)
    {
      std::string message = "Member '"+name+"' already exists on DGNode '"+dgNode+"'.";
      return LoggingImpl::reportError(message, errorOut);
    }
  }

  // check the name
  if(!isValidName(name, "Member"))
    return false;

  try
  {
    node.addMember_Variant(name.c_str(), rt.c_str(), defaultValue);

    // Initialize the Object here immedietly because persistence will load data into the value
    FabricCore::RTVal memberRTVal = node.getMemberSliceValue(name.c_str(), 0);
    if(memberRTVal.isObject() && memberRTVal.isNullObject() && autoInitObjects)
    {
      memberRTVal = FabricSplice::constructObjectRTVal(rt.c_str());
      if(!memberRTVal.isNullObject())
        node.setMemberSliceValue(name.c_str(), 0, memberRTVal);
    }
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }


  requireDGCheck();
  requireEvaluate();
  return true;
}

bool DGGraphImpl::hasDGNodeMember(const std::string & name, const std::string & dgNode)
{
  if(!hasDGNode(dgNode))
    return false;

  FabricCore::DGNode node = getDGNode(dgNode);
  FabricCore::Variant members = node.getMembers_Variant();

  for(FabricCore::Variant::DictIter keyIter(members); !keyIter.isDone(); keyIter.next())
  {
    std::string key = keyIter.getKey()->getStringData();
    if(key == name)
      return true;
  }

  return false;
}

std::string DGGraphImpl::getDGNodeMemberDataType(const std::string & name, const std::string & dgNode)
{
  if(hasDGNode(dgNode))
  {
    FabricCore::DGNode node = getDGNode(dgNode);
    FabricCore::Variant members = node.getMembers_Variant();

    for(FabricCore::Variant::DictIter keyIter(members); !keyIter.isDone(); keyIter.next())
    {
      std::string key = keyIter.getKey()->getStringData();
      if(key == name)
      {
        const FabricCore::Variant * dict = keyIter.getValue();
        return dict->getDictValue("type")->getStringData();
      }
    }
  }

  return "";
}

bool DGGraphImpl::removeDGNodeMember(
  const std::string & name, 
  const std::string & dgNode, 
  std::string * errorOut
) {
  FabricCore::DGNode node = getDGNode(dgNode);
  if(!node.isValid())
  {
    std::string message = "DGNode '"+dgNode+"' does not exist.";
    return LoggingImpl::reportError(message, errorOut);
  }

  FabricCore::Variant members = node.getMembers_Variant();

  bool found = false;
  for(FabricCore::Variant::DictIter keyIter(members); !keyIter.isDone(); keyIter.next())
  {
    std::string key = keyIter.getKey()->getStringData();
    if(key == name)
    {
      found = true;
      break;
    }
  }

  if(!found)
  {
    std::string message = "Member '"+name+"' does not exist on DGNode '"+dgNode+"'.";
    return LoggingImpl::reportError(message, errorOut);
  }

  // remove all ports on this dgnode
  stringVector portsToRemove;
  for(DGPortIt portIt=mDGPorts.begin();portIt!=mDGPorts.end();portIt++)
  {
    if(portIt->second->getDGNodeName() == dgNode)
    {
      if(portIt->second->getMember() == name)
        portsToRemove.push_back(portIt->first);
    }
  }
  for(size_t i=0;i<portsToRemove.size();i++)
    removeDGPort(portsToRemove[i]);

  try
  {
    node.removeMember(name.c_str());
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  requireDGCheck();
  requireEvaluate();
  return true;
}

DGPortImplPtr DGGraphImpl::addDGPort(
  DGGraphImplPtr thisGraph,
  const std::string & name, 
  const std::string & member, 
  DGPortImpl::Mode mode,
  const std::string & dgNode,
  bool autoInitObjects,
  std::string * errorOut
) {
  if(!thisGraph)
  {
    LoggingImpl::reportError("Parameter thisGraph is not valid.", errorOut);
    return DGPortImplPtr();
  }
  if(thisGraph.get() != this)
  {
    LoggingImpl::reportError("Parameter thisGraph does not refer to this graph.", errorOut);
    return DGPortImplPtr();
  }

  if(!isValidName(name, "Port"))
    return DGPortImplPtr();

  if(mDGPorts.find(name) != mDGPorts.end())
  {
    std::string message = "Port '"+name+"' already exists.";
    LoggingImpl::reportError(message, errorOut);
    return DGPortImplPtr();
  }

  std::string dgNodeName = dgNode;
  if(dgNodeName.length() == 0)
    dgNodeName = mDGNodeDefaultName;

  FabricCore::DGNode node = getDGNode(dgNodeName);
  if(!node.isValid())
  {
    std::string message = "DGNode '"+dgNodeName+"' does not exist.";
    LoggingImpl::reportError(message, errorOut);
    return DGPortImplPtr();
  }

  if(!hasDGNodeMember(member, dgNodeName))
  {
    std::string message = "DGNode '"+dgNodeName+"' does not contain member '"+member+"'.";
    LoggingImpl::reportError(message, errorOut);
    return DGPortImplPtr();
  }

  std::string dataType = getDGNodeMemberDataType(member, dgNodeName);
  bool isArray = false;
  if(StringUtilityImpl::endsWith(dataType, "[]")) 
  {
    dataType = dataType.substr(0, dataType.find('['));
    isArray = true;
  }

  uint32_t dataSize = GetRegisteredTypeSize(*sClient, dataType.c_str());
  bool shallow = GetRegisteredTypeIsShallow(*sClient, dataType.c_str());
  DGPortImpl * port = new DGPortImpl(thisGraph, name, member, node, dgNodeName, mode, dataSize, shallow, autoInitObjects);
  DGPortImplPtr portPtr(port);
  mDGPorts.insert(DGPortPair(name, portPtr));

  LoggingImpl::log("DGGraph '"+getName()+"' constructed new DGPort '"+name+"'.");

  validateAllKLOperators(errorOut);
  requireEvaluate();
  return portPtr;
}

bool DGGraphImpl::removeDGPort(const std::string & name, std::string * errorOut)
{
  DGPortIt it = mDGPorts.find(name);
  if(it == mDGPorts.end())
  {
    std::string message = "Port '"+name+"' does not exist.";
    return LoggingImpl::reportError(message, errorOut);
  }

  // remove all operators using this port
  for(size_t i=0;i<mBindings.size();i++)
  {
    DGBindingData & data = mBindings[i];
    if(!data.valid)
      continue;
    for(size_t j=0;j<data.portName.size();j++)
    {
      if(data.portName[j] == name)
      {
        data.valid = false;
        if(!mIsClearing)
        {
          FabricCore::DGNode node = sAllDGGraphs[i]->getDGNode(data.dgNode);
          FabricCore::DGBindingList bindings = node.getBindingList();
          FabricCore::DGBinding binding = bindings.getBinding(data.index);
          FabricCore::DGOperator op = binding.getOperator();
          LoggingImpl::log(std::string("KLOperator '")+op.getName()+"' on DGNode '"+data.dgNode+"' invalidated.");
        }
      }
    }
  }

  mDGPorts.erase(it);

  LoggingImpl::log("DGGraph '"+getName()+"' removed DGPort '"+name+"'.");
  requireEvaluate();
  return true;
}

DGPortImplPtr DGGraphImpl::getDGPort(const std::string & name)
{
  DGPortIt it = mDGPorts.find(name);
  if(it == mDGPorts.end())
    return DGPortImplPtr();
  return it->second;
}

char const * DGGraphImpl::getDGPortName(unsigned int index)
{
  if(index >= getDGPortCount())
    return "";
  DGPortIt it = mDGPorts.begin();
  std::advance(it, index);
  return it->first.c_str();
}

stringVector DGGraphImpl::getDGPortNames()
{
  stringVector result;
  for(DGPortIt it = mDGPorts.begin(); it != mDGPorts.end(); it++)
    result.push_back(it->second->getName());  
  return result;
}

DGPortImplPtr DGGraphImpl::getDGPort(unsigned int index)
{
  if(index >= getDGPortCount())
    return DGPortImplPtr();
  DGPortIt it = mDGPorts.begin();
  std::advance(it, index);
  return it->second;
}

unsigned int DGGraphImpl::getDGPortCount()
{
  return (unsigned int)mDGPorts.size();
}

bool DGGraphImpl::hasDGNodeDependency(const std::string & dgNode, const std::string & dependency, std::string * errorOut)
{
  FabricCore::DGNode nodeA = getDGNode(dgNode);
  if(!nodeA.isValid())
    return LoggingImpl::reportError("DGNode '"+dgNode+"' does not exist.", errorOut);
  FabricCore::DGNode nodeB = getDGNode(dependency);
  if(!nodeB.isValid())
    return LoggingImpl::reportError("DGNode '"+dgNode+"' does not exist.", errorOut);

  FabricCore::Variant dgDependenciesVar = nodeA.getDependencies_Variant();
  if(dgDependenciesVar.isDict())
  {
    for(FabricCore::Variant::DictIter keyIter(dgDependenciesVar); !keyIter.isDone(); keyIter.next())
    {
      std::string key = keyIter.getKey()->getStringData();
      if(key == dependency)
        return true;
    }
  }
  return false;
}

bool DGGraphImpl::setDGNodeDependency(const std::string & dgNode, const std::string & dependency, std::string * errorOut)
{
  if(dgNode == dependency)
    return LoggingImpl::reportError("DGNode '"+dgNode+"' and the dependency are the same.", errorOut);

  FabricCore::DGNode nodeA = getDGNode(dgNode);
  if(!nodeA.isValid())
    return LoggingImpl::reportError("DGNode '"+dgNode+"' does not exist.", errorOut);
  FabricCore::DGNode nodeB = getDGNode(dependency);
  if(!nodeB.isValid())
    return LoggingImpl::reportError("DGNode '"+dgNode+"' does not exist.", errorOut);

  if(hasDGNodeDependency(dgNode, dependency))
    return LoggingImpl::reportError("DGNode '"+dgNode+"' already depends on DGNode '"+dependency+"'.", errorOut);

  try
  {
    nodeA.setDependency(dependency.c_str(), nodeB);
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  LoggingImpl::log("DGGraph '"+getName()+"' constructed new DGNodeDependency '"+dgNode+"-->"+dependency+"'.");

  return true;
}

bool DGGraphImpl::removeDGNodeDependency(const std::string & dgNode, const std::string & dependency, std::string * errorOut)
{
  FabricCore::DGNode nodeA = getDGNode(dgNode);
  if(!nodeA.isValid())
    return LoggingImpl::reportError("DGNode '"+dgNode+"' does not exist.", errorOut);
  FabricCore::DGNode nodeB = getDGNode(dependency);
  if(!nodeB.isValid())
    return LoggingImpl::reportError("DGNode '"+dgNode+"' does not exist.", errorOut);

  if(!hasDGNodeDependency(dgNode, dependency))
    return LoggingImpl::reportError("DGNode '"+dgNode+"' doesn't depend on DGNode '"+dependency+"'.", errorOut);

  // remove all operators using this dependency
  FabricCore::DGBindingList bindings = nodeA.getBindingList();
  std::vector<uint32_t> bindingsToRemove;
  for(uint32_t i=0;i<bindings.getCount();i++)
  {
    FabricCore::DGBinding binding = bindings.getBinding(i);
    FabricCore::Variant paramLayout = binding.getParameterLayout_Variant();
    if(!paramLayout.isArray())
      continue;
    for(uint32_t j=0;j<paramLayout.getArraySize();j++)
    {
      stringVector parts = StringUtilityImpl::splitString(paramLayout.getArrayElement(j)->getStringData(), '.');
      if(parts.size() < 1)
        continue;
      if(parts[0] == dependency)
        bindingsToRemove.push_back(i);
    }
  }
  for(int j=bindingsToRemove.size()-1;j>=0;j--)
  {
    FabricCore::DGBinding binding = bindings.getBinding(bindingsToRemove[j]);
    FabricCore::DGOperator op = binding.getOperator();
    std::string opName = op.getName();
    LoggingImpl::log("DGGraph '"+getName()+"' removed operator '"+opName+"'.");
    bindings.remove(bindingsToRemove[j]);
  }

  try
  {
    nodeA.removeDependency(dependency.c_str());
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  LoggingImpl::log("DGGraph '"+getName()+"' removed DGNodeDependency '"+dgNode+"-->"+dependency+"'.");

  return true;
}

bool DGGraphImpl::checkErrorVariant(const FabricCore::Variant * variant, std::string * errorOut)
{
  if(variant->isNull())
  {
    return true;
  }
  if(variant->isString())
  {
    std::string message = variant->getStringData();
    return LoggingImpl::reportError(message, errorOut);
  }
  else if(variant->isArray())
  {
    bool result = true;
    for(uint32_t i=0;i<variant->getArraySize();i++)
      if(!checkErrorVariant(variant->getArrayElement(i), errorOut))
        result = false;
    return result;
  }
  else
  {
    const FabricCore::Variant * column = variant->getDictValue("column");
    const FabricCore::Variant * line = variant->getDictValue("line");
    const FabricCore::Variant * filename = variant->getDictValue("filename");
    const FabricCore::Variant * level = variant->getDictValue("level");
    const FabricCore::Variant * desc = variant->getDictValue("desc");
    const FabricCore::Variant * prefixes = variant->getDictValue("prefixes");
    const FabricCore::Variant * suffixes = variant->getDictValue("suffixes");

    if ( prefixes )
      checkErrorVariant(prefixes, errorOut);

    LoggingImpl::reportCompilerError(
      line ? line->getUInt32() : 0,
      column ? column->getUInt32() : 0,
      filename ? filename->getStringData() : "",
      level ? level->getStringData() : "",
      desc ? desc->getStringData() : ""
    );

    if ( suffixes )
      checkErrorVariant(suffixes, errorOut);

    return false;
  }
  return true;
}

bool DGGraphImpl::checkErrors(std::string * errorOut)
{
  if(!sDGCheckRequired)
    return true;

  bool result = true;

  // loop over all operators
  for(DGOperatorIt opIt = sDGOperators.begin(); opIt != sDGOperators.end(); opIt++)
  {
    FabricCore::Variant feedback = opIt->second.op.getErrors();
    if(!checkErrorVariant(&feedback, errorOut))
      result = false;
    feedback = opIt->second.op.getDiagnostics();
    if(!checkErrorVariant(&feedback, errorOut))
      result = false;
  }

  // loop over all nodes and validate their bindings
  for(size_t i=0;i<sAllDGGraphs.size();i++)
  {
    DGNodeMap & dgNodeMap = sAllDGGraphs[i]->mDGNodes;
    for(DGNodeIt nodeIt = dgNodeMap.begin(); nodeIt != dgNodeMap.end(); nodeIt++)
    {
      FabricCore::Variant feedback = nodeIt->second.node.getErrors();
      if(!checkErrorVariant(&feedback, errorOut))
        result = false;

      FabricCore::DGBindingList bindings = nodeIt->second.node.getBindingList();
      for(uint32_t k=0;k<bindings.getCount();k++)
      {
        FabricCore::DGBinding binding = bindings.getBinding(k);
        feedback = binding.getErrors();
        if(!checkErrorVariant(&feedback, errorOut))
          result = false;
      }
    }
  }

  sDGCheckRequired = false;
  return result;
}

std::string DGGraphImpl::generateKLOperatorParameterList(bool useLineBreaks)
{
  std::string result;
  for(DGPortIt it=mDGPorts.begin();it!=mDGPorts.end();it++)
  {
    if(result.length() > 0)
      result += ", ";
    if(useLineBreaks)
      result += "\n  ";
    if(it->second->getMode() == DGPortImpl::Mode_IN)
      result += "in ";
    else
      result += "io ";

    stringVector parts = StringUtilityImpl::partitionString(it->second->getDataType(), '[');
    result += parts[0];
    result += " ";
    result += it->second->getName();
    result += parts[1];
    result += parts[2];
    if(it->second->isArray())
      result += "[]";
  }
  return result;
}

std::string DGGraphImpl::generateKLOperatorSourceCode(
  const std::string & name, 
  const std::string & additionalBody, 
  const std::string & additionalFunctions,
  const std::string & executeParallelMember
) {
  std::string code;

  std::string params = generateKLOperatorParameterList(true);
  std::string requireCode = "require Math;\n";
  std::string bodyCode;
  std::string executeParallelFunction;
  std::string executeParallelBody;

  std::map<std::string, bool> usedTypes;
  usedTypes.insert(std::pair<std::string, bool>("Boolean", true));
  usedTypes.insert(std::pair<std::string, bool>("Byte", true));
  usedTypes.insert(std::pair<std::string, bool>("UInt8", true));
  usedTypes.insert(std::pair<std::string, bool>("UInt16", true));
  usedTypes.insert(std::pair<std::string, bool>("UInt32", true));
  usedTypes.insert(std::pair<std::string, bool>("UInt64", true));
  usedTypes.insert(std::pair<std::string, bool>("SInt8", true));
  usedTypes.insert(std::pair<std::string, bool>("SInt16", true));
  usedTypes.insert(std::pair<std::string, bool>("SInt32", true));
  usedTypes.insert(std::pair<std::string, bool>("SInt64", true));
  usedTypes.insert(std::pair<std::string, bool>("Integer", true));
  usedTypes.insert(std::pair<std::string, bool>("Size", true));
  usedTypes.insert(std::pair<std::string, bool>("Index", true));
  usedTypes.insert(std::pair<std::string, bool>("Float32", true));
  usedTypes.insert(std::pair<std::string, bool>("Float64", true));
  usedTypes.insert(std::pair<std::string, bool>("Scalar", true));
  usedTypes.insert(std::pair<std::string, bool>("String", true));

  usedTypes.insert(std::pair<std::string, bool>("Math", true));
  usedTypes.insert(std::pair<std::string, bool>("Vec2", true));
  usedTypes.insert(std::pair<std::string, bool>("Vec3", true));
  usedTypes.insert(std::pair<std::string, bool>("Vec4", true));
  usedTypes.insert(std::pair<std::string, bool>("Quat", true));
  usedTypes.insert(std::pair<std::string, bool>("Mat22", true));
  usedTypes.insert(std::pair<std::string, bool>("Mat33", true));
  usedTypes.insert(std::pair<std::string, bool>("Mat44", true));
  usedTypes.insert(std::pair<std::string, bool>("Euler", true));
  usedTypes.insert(std::pair<std::string, bool>("RotationOrder", true));
  usedTypes.insert(std::pair<std::string, bool>("Ray", true));
  usedTypes.insert(std::pair<std::string, bool>("Color", true));
  usedTypes.insert(std::pair<std::string, bool>("RGB", true));
  usedTypes.insert(std::pair<std::string, bool>("RGBA", true));
  usedTypes.insert(std::pair<std::string, bool>("ARGB", true));

  for(DGPortIt it=mDGPorts.begin();it!=mDGPorts.end();it++)
  {
    std::string portName = it->second->getName();
    std::string dataType = it->second->getDataType();
    dataType = StringUtilityImpl::partitionString(dataType, '[')[0];

    try
    {
      const char * extName = FabricCore::GetRegisteredTypeExtName(*sClient, dataType.c_str());
      if(extName != NULL)
      {
        if(extName != dataType)
        {
          dataType = extName;
        }
      }
    }
    catch(FabricCore::Exception e)
    {
    }

    if(usedTypes.find(dataType) != usedTypes.end())
      continue;

    requireCode += "require "+dataType+";\n";
    usedTypes.insert(std::pair<std::string, bool>(dataType, true));
  }

  if(executeParallelMember.length() > 0)
  {
    DGPortImplPtr port = getDGPort(executeParallelMember);
    if(port)
    {
      std::string dataType = port->getDataType();
      if(port->isArray())
      {
        executeParallelFunction += "operator "+executeParallelMember+"_task<<<index>>>";
        executeParallelFunction += "(io "+dataType+" "+executeParallelMember+"[]) {\n";
        executeParallelFunction += "  // perform per element code here\n";
        executeParallelFunction += "}\n\n";

        executeParallelBody += "\n";
        executeParallelBody += "  "+executeParallelMember+"_task<<<"+executeParallelMember+".size()>>>";
        executeParallelBody += "("+executeParallelMember+");\n";
        executeParallelBody += "\n";
      }
      else if(dataType == "PolygonMesh")
      {
        executeParallelFunction += "operator "+executeParallelMember+"_task<<<index>>>";
        executeParallelFunction += "(io "+dataType+" "+executeParallelMember+") {\n";
        executeParallelFunction += "  // perform per vertex code here\n";
        executeParallelFunction += "  Vec3 pos = "+executeParallelMember+".getPointPosition(index);\n";
        executeParallelFunction += "  "+executeParallelMember+".setPointPosition(index, pos);\n";
        executeParallelFunction += "}\n\n";

        executeParallelBody += "\n";
        executeParallelBody += "  if("+executeParallelMember+" != null)";
        executeParallelBody += "    "+executeParallelMember+"_task<<<"+executeParallelMember+".pointCount()>>>";
        executeParallelBody += "("+executeParallelMember+");\n";
        executeParallelBody += "\n";
      }
    }
  }

  code += requireCode + "\n";
  code += executeParallelFunction;
  code += additionalFunctions;
  code += "operator "+name+"("+params+"\n) {\n";
  code += bodyCode;
  code += executeParallelBody;
  code += additionalBody;
  code += "  \n";
  code += "}\n";

  return code;
}

bool DGGraphImpl::constructKLOperator(
  const std::string & name,
  const std::string & sourceCode,
  const std::string & entry,
  const std::string & dgNode, 
  const FabricCore::Variant & opPortMap,
  std::string * errorOut
) {
  std::string dgNodeName = dgNode;
  if(dgNodeName.length() == 0)
    dgNodeName = mDGNodeDefaultName;

  if(mDGOperatorNameMap.find(name) != mDGOperatorNameMap.end())
    return LoggingImpl::reportError("DGNode '"+dgNodeName+"' already contains an operator called '"+name+"'.", errorOut);

  FabricCore::DGNode node = getDGNode(dgNodeName);
  if(!node.isValid())
    return LoggingImpl::reportError("DGNode '"+dgNodeName+"' does not exist.", errorOut);

  if(!isValidName(name, "Operator"))
    return false;

  std::string entryFunction = name;
  if(entry.length() > 0)
    entryFunction = entry;

  // check the local map

  std::string klCode = sourceCode;
  std::string tempCode = "operator "+entryFunction+"() {\n  \n}";

  std::string opName = name;

  DGOperatorSuffixIt suffixIt = sDGOperatorSuffix.find(opName);
  if(suffixIt == sDGOperatorSuffix.end())
  {
    sDGOperatorSuffix.insert(DGOperatorSuffixPair(name, 1));
  }
  else
  {
#ifdef _WIN32
    char buffer[32];
    _itoa_s(suffixIt->second, buffer, 32, 10);
    opName += std::string("_") + std::string(buffer);
#else
    std::stringstream ss;
    ss << suffixIt->second;
    opName += std::string("_") + ss.str();
#endif
    suffixIt->second++;
  }

  mDGOperatorNameMap.insert(stringPair(name, opName));

  FabricCore::DGOperator op;
  try
  {
    op = FabricCore::DGOperator(*sClient, opName.c_str(), (opName+".kl").c_str(), tempCode.c_str(), entryFunction.c_str());
    LoggingImpl::log("DGGraph '"+getName()+"' constructed new KL Operator '"+name+"'.");
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  sDGOperators.insert(DGOperatorPair(opName, DGOperatorData(op, entryFunction, klCode)));

  FabricCore::DGBinding binding(op, 0, 0);

  try
  {
    DGBindingData bindingData;
    bindingData.valid = true;
    bindingData.opName = name;
    bindingData.dgNode = dgNodeName;
    bindingData.index = node.getBindingList().getCount();
    bindingData.portName.clear();
    mBindings.push_back(bindingData);

    node.appendBinding(binding);
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  LoggingImpl::log("DGGraph '"+getName()+"' bound new KL Operator '"+name+"'.");

  FabricCore::Variant portMapClone = FabricCore::Variant::CreateFromJSON(opPortMap.getJSONEncoding().getStringData());
  mDGNodes.find(dgNodeName)->second.opPortMaps.push_back(portMapClone);
  if(klCode.length() > 0)
    return setKLOperatorSourceCode(name, klCode, entry, errorOut);

  requireDGCheck();
  requireEvaluate();
  return true;
}

bool DGGraphImpl::removeKLOperator(const std::string & name, const std::string & dgNode, std::string * errorOut)
{
  std::string dgNodeName = dgNode;
  if(dgNodeName.length() == 0)
    dgNodeName = mDGNodeDefaultName;

  std::string opName = name;
  stringIt it = mDGOperatorNameMap.find(name);
  if(it != mDGOperatorNameMap.end())
  {
    opName = it->second;
    mDGOperatorNameMap.erase(it);
  }
  else
  {
    for(it = mDGOperatorNameMap.begin(); it != mDGOperatorNameMap.end(); it++)
    {
      if(it->second == name)
      {
        mDGOperatorNameMap.erase(it);
        break;
      }
    }
  }

  FabricCore::DGNode node = getDGNode(dgNodeName);
  if(!node.isValid())
  {
    LoggingImpl::reportError("DGNode '"+dgNodeName+"' does not exist.", errorOut);
    return false;
  }

  bool result = false;
  FabricCore::DGBindingList bindings = node.getBindingList();
  std::vector<size_t> bindingsToRemove;
  for(int i=(int)bindings.getCount()-1;i!=-1;i--)
  {
    FabricCore::DGBinding binding = bindings.getBinding(i);
    FabricCore::DGOperator op = binding.getOperator();
    if(op.getName() == opName)
    {
      bindings.remove(i);
      for(size_t j=0;j<mBindings.size();j++)
      {
        if(dgNodeName == mBindings[j].dgNode && (size_t)i == mBindings[j].index)
        {
          bindingsToRemove.push_back(j);
          break;
        }
      }
      DGNodeIt it = mDGNodes.find(dgNodeName);
      it->second.opPortMaps.erase(it->second.opPortMaps.begin() + i);
      result = true;
    }
  }

  for(int i=bindingsToRemove.size()-1; i>=0; i--) 
  {
    size_t index = bindingsToRemove[i];
    for(size_t j=0;j<mBindings.size();j++)
    {
      if(mBindings[j].dgNode == dgNodeName && mBindings[j].index > mBindings[index].index)
        mBindings[j].index--;
    }
    mBindings.erase(mBindings.begin() + index);
  }

  requireEvaluate();

  if(result)
  {
    DGOperatorIt it = sDGOperators.find(opName);
    if(it != sDGOperators.end())
    {
      it->second.op.destroy();
      sDGOperators.erase(it);
    }
    DGOperatorSuffixIt suffixIt = sDGOperatorSuffix.find(opName);
    if(suffixIt != sDGOperatorSuffix.end())
    {
      if(suffixIt->second == 1)
      {
        sDGOperatorSuffix.erase(suffixIt);
      }
    }
    LoggingImpl::log("DGGraph '"+getName()+"' removed KL Operator '"+name+"'.");
  }

  return result;
}

bool DGGraphImpl::hasKLOperator(const std::string & name, const std::string & dgNodeName, std::string * errorOut)
{
  return mDGOperatorNameMap.find(name) != mDGOperatorNameMap.end();
}

char const * DGGraphImpl::getKLOperatorEntry(const std::string & name, std::string * errorOut)
{
  std::string opName = getRealDGOperatorName(name.c_str());
  DGOperatorIt opIt = sDGOperators.find(opName);
  if(opIt == sDGOperators.end())
  {
    LoggingImpl::reportError("Operator '"+name+"' doesn't exist.", errorOut);
    return "";
  }
  return opIt->second.entry.c_str();
}

bool DGGraphImpl::setKLOperatorEntry(const std::string & name, const std::string & entry, std::string * errorOut)
{
  std::string opName = getRealDGOperatorName(name.c_str());
  DGOperatorIt opIt = sDGOperators.find(opName);
  if(opIt == sDGOperators.end())
  {
    LoggingImpl::reportError("Operator '"+name+"' doesn't exist.", errorOut);
    return "";
  }

  FabricCore::DGOperator op = opIt->second.op;
  opIt->second.entry = entry;
  op.setEntryPoint(entry.c_str());

  LoggingImpl::log("KL Operator '"+name+"' entry updated.");

  requireDGCheck();
  return checkErrors(errorOut);
}

bool DGGraphImpl::setKLOperatorIndex(const std::string & name, unsigned int index, std::string * errorOut)
{
  std::string opName = getRealDGOperatorName(name.c_str());
  DGOperatorIt opIt = sDGOperators.find(opName);
  if(opIt == sDGOperators.end())
  {
    LoggingImpl::reportError("Operator '"+name+"' doesn't exist.", errorOut);
    return false;
  }

  if(index >= mBindings.size())
  {
    LoggingImpl::reportError("New index for Operator '"+name+"' is out of bounds.", errorOut);
    return false;
  }

  FabricCore::DGOperator op = opIt->second.op;

  DGBindingData * prevBindingData = NULL;
  DGBindingData * newBindingData = NULL;

  for(size_t i=0;i<mBindings.size();i++)
  {
    FabricCore::DGNode node = getDGNode(mBindings[i].dgNode);
    FabricCore::DGBindingList bindings = node.getBindingList();
    FabricCore::DGBinding binding = bindings.getBinding(mBindings[i].index);
    if(binding.getOperator().getName() == opName && prevBindingData == NULL)
    {
      prevBindingData = &mBindings[i];
    }
    if(mBindings[i].index == index && newBindingData == NULL)
    {
      newBindingData = &mBindings[i];
    }
  }

  if(prevBindingData == NULL)
  {
    LoggingImpl::reportError("Operator '"+name+"' not found on stack of node "+getName()+".", errorOut);
    return false;
  }

  if(newBindingData == NULL)
  {
    LoggingImpl::reportError("Operator '"+name+"'cannot be moved, invalid index.", errorOut);
    return false;
  }

  unsigned int prevIndex = prevBindingData->index;
  unsigned int newIndex = index;

  if(prevIndex == newIndex)
  {
    LoggingImpl::reportError("Operator '"+name+"' already at the given index.", errorOut);
    return false;
  }

  FabricCore::DGNode node = getDGNode(prevBindingData->dgNode);
  FabricCore::DGBindingList bindings = node.getBindingList();
  FabricCore::DGBinding binding = bindings.getBinding(prevIndex);
  bindings.remove(prevIndex);
  if(newIndex > prevIndex + 1)
    bindings.insert(binding, newIndex--);
  else
    bindings.insert(binding, newIndex);

  std::vector<DGBindingData> newBindings(bindings.getCount());
  for(size_t i=0;i<newBindings.size();i++)
  {
    FabricCore::DGBinding binding = bindings.getBinding(i);
    newBindings[i].valid = true;
    newBindings[i].dgNode = "DGNode";
    newBindings[i].index = i;
    newBindings[i].opName = binding.getOperator().getName();

    for(size_t j=0;j<mBindings.size();j++)
    {
      if(mBindings[j].opName == newBindings[i].opName)
      {
        newBindings[i].portName = mBindings[j].portName;
        break;
      }
    }
  }

  mBindings = newBindings;

  LoggingImpl::log("KL Operator '"+name+"' moved.");
  return checkErrors(errorOut);
}

char const * DGGraphImpl::getKLOperatorSourceCode(const std::string & name, std::string * errorOut)
{
  return getKLOperatorSourceCodeByRealOpName(getRealDGOperatorName(name.c_str()), errorOut);
}

char const * DGGraphImpl::getKLOperatorSourceCodeByRealOpName(const std::string & realOpName, std::string * errorOut)
{
  DGOperatorIt opIt = sDGOperators.find(realOpName);
  if(opIt == sDGOperators.end())
  {
    LoggingImpl::reportError("Operator doesn't exist.", errorOut);
    return "";
  }
  return opIt->second.klCode.c_str();
}

bool DGGraphImpl::setKLOperatorSourceCode(
  const std::string & name, 
  const std::string & sourceCode, 
  const std::string & entry, 
  std::string * errorOut
) {

  std::string entryFunction = name;
  if(entry.length() > 0)
    entryFunction = entry;

  std::string opName = getRealDGOperatorName(name.c_str());
  DGOperatorIt opIt = sDGOperators.find(opName);
  if(opIt == sDGOperators.end())
    return LoggingImpl::reportError("Operator '"+name+"' doesn't exist.", errorOut);

  FabricCore::DGOperator op = opIt->second.op;

  // always store the klCode in the map, even if it is invalid
  opIt->second.entry = entryFunction;
  opIt->second.klCode = sourceCode;

  // load all dependencies
  KLParserImplPtr parser = KLParserImpl::getParser(opName.c_str(), opName.c_str(), sourceCode.c_str());
  for(unsigned int i=0;i<parser->getNbKLRequires();i++)
  {
    std::string requiredType = parser->getKLRequire(i);
    if(sClientRTs.find(requiredType) != sClientRTs.end())
      continue;
    std::string rtError, extError;
    loadExtension(requiredType, &rtError);

    if(rtError.length() > 0 && extError.length() > 0)
    {
      std::string message = "Require statement '"+requiredType+"' in RT '"+name+"' cannot be resolved.";
      if(rtError.length() > 0) message += "\n" + rtError;
      if(extError.length() > 0) message += "\n" + extError;
      return LoggingImpl::reportError(message, errorOut);
    }
    LoggingImpl::clearError();
  }

  std::string entryPoint = entry;
  if(entryPoint.length() == 0)
    entryPoint = op.getEntryPoint();
  if(entryPoint.length() == 0)
    entryPoint = name;

  bool justCreated = opIt->second.params.size() == 0;
  opIt->second.params.clear();

  const KLParserImpl::KLOperator * parserOp = NULL;
  for(size_t i=0;i<parser->getNbKLOperators();i++)
  {
    parserOp = parser->getKLOperator(i);
    if(parserOp->name() == entryPoint)
      break;
    parserOp = NULL;
  }
  if(parserOp == NULL)
    return LoggingImpl::reportError("Entry function "+entryPoint+" not found in KL sourcecode.", errorOut);
  if(parserOp->isPex())
    return LoggingImpl::reportError("Entry function "+entryPoint+" found in KL sourcecode, but is uses PEX.", errorOut);

  const KLParserImpl::KLArgumentList * parserOpArgs = parserOp->arguments();
  for(unsigned int i=0;i<parserOpArgs->nbArgs();i++)
  {
    DGOperatorParamInfo info;
    info.mode = parserOpArgs->mode(i);
    info.name = parserOpArgs->name(i);
    info.dataType = parserOpArgs->type(i);
    info.memberType = info.dataType;
    if(StringUtilityImpl::endsWith(info.dataType, "[]"))
    {
      info.dataType = StringUtilityImpl::truncateRight(info.dataType, 2);
      info.isArray = true;
    }
    else
      info.isArray = false;

    if(StringUtilityImpl::endsWith(info.dataType, "<>"))
    {
      info.dataType = StringUtilityImpl::truncateRight(info.dataType, 2);
      info.isSliced = true;
    }
    else
      info.isSliced = false;

    info.binding = info.name;
    if(info.isSliced)
      info.binding += "<>";

    opIt->second.params.push_back(info);    
  }

  const char * tempFilePath = getenv("FABRIC_SPLICE_TEMP_KLFILE");
  if(tempFilePath != NULL)
  {
    FILE * tempFile = fopen(tempFilePath, "wb");
    if(tempFile)
    {
      fwrite(sourceCode.c_str(), sourceCode.length(), 1, tempFile);
      fclose(tempFile);
    }
    else
    {
      LoggingImpl::reportError("Temporary KL file specified in FABRIC_SPLICE_TEMP_KLFILE could not be written.", errorOut);
    }
  }

  try
  {
    op.setEntryPoint(entryPoint.c_str());
    op.setSourceCode((sourceCode+"").c_str());
    if(std::string(op.getFilename()).empty())
      op.setFilename((name+".kl").c_str());
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  if(!invalidateKLOperator(opName, errorOut))
    return false;
  if(!validateKLOperator(opName, !justCreated, errorOut))
    return false;

  LoggingImpl::log("KL Operator '"+name+"' sourcecode updated.");
  LoggingImpl::clearError();

  requireDGCheck();
  return checkErrors(errorOut);
}

void DGGraphImpl::loadKLOperatorSourceCode(const std::string & name, const std::string & filePath, std::string * errorOut)
{
  FILE * file = fopen(filePath.c_str(), "rb");
  if(!file)
  {
    LoggingImpl::reportError("Invalid filePath '"+filePath+"'", errorOut);
    return;
  }

  fseek( file, 0, SEEK_END );
  long fileSize = ftell( file );
  rewind( file );

  char * buffer = (char*) malloc(fileSize + 1);
  buffer[fileSize] = '\0';

  size_t readBytes = fread(buffer, 1, fileSize, file);
  assert(readBytes == size_t(fileSize));
  (void)readBytes;

  fclose(file);

  std::string code = buffer;
  free(buffer);

  std::string entryPoint = getKLOperatorEntry(name, errorOut);
  setKLOperatorSourceCode(name, code, entryPoint);
}

void DGGraphImpl::saveKLOperatorSourceCode(const std::string & name, const std::string & filePath, std::string * errorOut)
{
  std::string code = getKLOperatorSourceCode(name, errorOut);
  if(code.length() == 0)
    return;

  FILE * file = fopen(filePath.c_str(), "wb");
  if(!file)
  {
    LoggingImpl::reportError("Invalid filePath '"+filePath+"'", errorOut);
    return;
  }

  fwrite(code.c_str(), code.length(), 1, file);
  fclose(file);
}

bool DGGraphImpl::isKLOperatorFileBased(const std::string & name, std::string * errorOut)
{
  std::string opName = getRealDGOperatorName(name.c_str());
  return mKLOperatorFileNames.find(opName) != mKLOperatorFileNames.end();  
}

char const * DGGraphImpl::getKLOperatorFilePath(const std::string & name, std::string * errorOut)
{
  std::string opName = getRealDGOperatorName(name.c_str());
  if(mKLOperatorFileNames.find(opName) != mKLOperatorFileNames.end())
    return mKLOperatorFileNames.find(opName)->second.c_str();

  DGOperatorIt opIt = sDGOperators.find(opName);
  if(opIt == sDGOperators.end())
  {
    LoggingImpl::reportError("Operator '"+name+"' doesn't exist.", errorOut);
    return NULL;
  }

  return opIt->second.op.getFilename();
}

void DGGraphImpl::setKLOperatorFilePath(const std::string & name, const std::string & filePath, const std::string & entry, std::string * errorOut)
{
  std::string opName = getRealDGOperatorName(name.c_str());
  DGOperatorIt opIt = sDGOperators.find(opName);
  if(opIt == sDGOperators.end())
  {
    LoggingImpl::reportError("Operator '"+name+"' doesn't exist.", errorOut);
    return;
  }

  std::string resolvedFilePath = resolveEnvironmentVariables(filePath);
  std::ifstream file(resolvedFilePath.data());

  if(!file)
  {
    LoggingImpl::reportError("Invalid filePath '"+filePath+"'", errorOut);
    return;
  }

  if(mKLOperatorFileNames.find(opName) == mKLOperatorFileNames.end())
    mKLOperatorFileNames.insert(stringPair(opName, filePath));
  else
    mKLOperatorFileNames.find(opName)->second = filePath;
  
  std::stringstream buffer;
  buffer << file.rdbuf();

  std::string entryPoint = entry;
  if(entry.length() != 0)
    entryPoint = getKLOperatorEntry(name, errorOut);
  setKLOperatorSourceCode(name, buffer.str(), entryPoint); 

  FabricCore::DGOperator op = opIt->second.op;
  op.setFilename(filePath.c_str());
}

bool DGGraphImpl::invalidateKLOperator(const std::string & opName, std::string * errorOut)
{
  for(size_t i=0;i<sAllDGGraphs.size();i++)
  {
    for(size_t j=0;j<sAllDGGraphs[i]->mBindings.size();j++)
    {
      DGBindingData & data = sAllDGGraphs[i]->mBindings[j];
      FabricCore::DGNode node = sAllDGGraphs[i]->getDGNode(data.dgNode);
      FabricCore::DGBindingList bindings = node.getBindingList();
      FabricCore::DGBinding binding = bindings.getBinding(data.index);
      FabricCore::DGOperator op = binding.getOperator();
      if(op.getName() != opName)
        continue;
      data.valid = false;
    }
  }
  return true;
}

bool DGGraphImpl::isValidName(const std::string & name, const std::string & context, std::string * errorOut)
{
  // check keywords etc
  if(name == "index" || name == "name" || name == "self" || name == "null" || name == "context")
  {
      std::string message = context+" '"+name+"' uses a reserved keyword as the name.";
      return LoggingImpl::reportError(message, errorOut);
  }
  if(sClientRTs.find(name) != sClientRTs.end())
  {
      std::string message = context+" '"+name+"' uses a known RT as the name.";
      return LoggingImpl::reportError(message, errorOut);
  }
  for(int i=KLParserImpl::KLSymbol::Type_firstkeyword;i<KLParserImpl::KLSymbol::Type_lastkeyword;i++) {
    std::string keyWord = KLParserImpl::KLSymbol::typeNameFromType((KLParserImpl::KLSymbol::Type)i);
    if(keyWord == name)
    {
        std::string message = context+" '"+name+"' uses a known KL keyword as the name.";
        return LoggingImpl::reportError(message, errorOut);
    }
  }
  return true;
}

bool DGGraphImpl::validateAllKLOperators(std::string * errorOut)
{
  for(DGOperatorIt it = sDGOperators.begin(); it != sDGOperators.end(); it++)
  {
    if(!validateKLOperator(it->first, true, errorOut))
      return false;
  }
  return true;
}

bool DGGraphImpl::validateKLOperator(const std::string & opName, bool logValidation, std::string * errorOut)
{
  DGOperatorIt opIt = sDGOperators.find(opName);
  if(opIt == sDGOperators.end())
    return false;

  std::string sourceCode = getKLOperatorSourceCodeByRealOpName(opName, errorOut);
  if(sourceCode.size() == 0)
    return false;
 
  // now that we have the parameter infos
  // let's ensure all of the graphs we want to
  // put this into has the right members
  for(size_t i=0;i<sAllDGGraphs.size();i++)
  {
    for(size_t j=0;j<sAllDGGraphs[i]->mBindings.size();j++)
    {
      DGBindingData & data = sAllDGGraphs[i]->mBindings[j];
      if(data.valid)
        continue;
      data.portName.clear();
      data.portName.resize(opIt->second.params.size());

      DGNodeData & nodeData = sAllDGGraphs[i]->mDGNodes.find(data.dgNode)->second;
      FabricCore::DGBindingList bindings = nodeData.node.getBindingList();
      FabricCore::DGBinding binding = bindings.getBinding(data.index);
      FabricCore::DGOperator op = binding.getOperator();
      if(op.getName() != opName)
        continue;

      // if there's only on operator, reset the mUsesEvalContext
      if(bindings.getCount() == 1)
        sAllDGGraphs[i]->mUsesEvalContext = false;

      for(size_t k=0;k<opIt->second.params.size();k++)
      {
        // special index member
        if(opIt->second.params[k].name == "index" && opIt->second.params[k].dataType == "Index" && !opIt->second.params[k].isArray && !opIt->second.params[k].isSliced)
          continue;

        // special context member
        if(opIt->second.params[k].name == "context" && opIt->second.params[k].dataType == "EvalContext" && !opIt->second.params[k].isArray && !opIt->second.params[k].isSliced)
        {
          data.portName[k] = opIt->second.params[k].name;
          sAllDGGraphs[i]->mUsesEvalContext = true;
          continue;
        }

        // container data type
        if(opIt->second.params[k].dataType == "Container" && !opIt->second.params[k].isArray && !opIt->second.params[k].isSliced)
        {
          if(opIt->second.params[k].name != "self")
          {
            LoggingImpl::reportError(
              "Cannot update operator '"+opName+"', Containers other than 'self' are not supported." , errorOut);
            LoggingImpl::clearError();
            continue;
          }
        }

        // check if the port name has to be remapped
        data.portName[k] = opIt->second.params[k].name;
        if(data.index < nodeData.opPortMaps.size())
        {
          if(nodeData.opPortMaps[data.index].isDict())
          {
            const FabricCore::Variant * portMapVar = nodeData.opPortMaps[data.index].getDictValue(data.portName[k].c_str());
            if(portMapVar)
            {
              if(portMapVar->isString())
                data.portName[k] = portMapVar->getStringData();
            }
          }
        }

        // check if the port exists
        DGPortImplPtr port = sAllDGGraphs[i]->getDGPort(data.portName[k]);
        if(!port)
        {
          LoggingImpl::reportError(
            "Cannot update operator '"+opName+"', DGNode '"+
            nodeData.node.getName()+"' doesn't have a port '"+data.portName[k]+"'" , errorOut);
          LoggingImpl::clearError();
          data.portName[k] = "";
          continue;
        }

        // check dependencies
        if(port->getDGNodeName() != data.dgNode)
        {
          if(!sAllDGGraphs[i]->hasDGNodeDependency(data.dgNode, port->getDGNodeName()))
          {
            LoggingImpl::reportError(
              "Cannot update operator '"+opName+"', DGNode '"+
              nodeData.node.getName()+"' doesn't have a dependency to '"+port->getDGNodeName()+"'" , errorOut);
            LoggingImpl::clearError();
            data.portName[k] = "";
            continue;
          }
        }

        std::string portDataType = port->getDataType();
        if(port->isArray())
          portDataType += "[]";
        if(portDataType != opIt->second.params[k].memberType)
        {
          LoggingImpl::reportError(
            "Cannot update operator '"+opName+"', the port '"+data.portName[k]+
            "' is not of type '"+opIt->second.params[k].memberType+"', but '"+portDataType+"'." , errorOut);
          LoggingImpl::clearError();
          data.portName[k] = "";
        continue;
        }
      }
    }
  }

  // by now we know that all nodes utilizing this 
  // operator have the right data layout, so let's update the bindings
  for(size_t i=0;i<sAllDGGraphs.size();i++)
  {
    for(size_t j=0;j<sAllDGGraphs[i]->mBindings.size();j++)
    {
      DGBindingData & data = sAllDGGraphs[i]->mBindings[j];
      if(data.valid)
        continue;
      DGNodeData & nodeData = sAllDGGraphs[i]->mDGNodes.find(data.dgNode)->second;
      FabricCore::DGBindingList bindings = nodeData.node.getBindingList();
      FabricCore::DGBinding binding = bindings.getBinding(data.index);
      FabricCore::DGOperator op = binding.getOperator();
      if(op.getName() != opName)
        continue;

      bool allPortsValid = true;
      for(size_t k=0;k<opIt->second.params.size();k++)
      {
        if(data.portName[k].length() == 0)
        {
          allPortsValid = false;
          break;
        }
      }

      if(!allPortsValid)
        continue;

      stringVector parameterLayoutStr(opIt->second.params.size());
      char const ** parameterLayoutChar = (char const **)malloc(sizeof(char const *) * opIt->second.params.size());
      for(size_t k=0;k<opIt->second.params.size();k++)
      {
        std::string boundDGNode = "self";

        // special context member
        if(opIt->second.params[k].name == "context" && opIt->second.params[k].dataType == "EvalContext" && !opIt->second.params[k].isArray && !opIt->second.params[k].isSliced)
        {
          parameterLayoutStr[k] = boundDGNode + "." + opIt->second.params[k].name;
          parameterLayoutChar[k] = parameterLayoutStr[k].c_str();
          continue;
        }

        // check the port's dgnode
        // for an eventual dependency
        DGPortImplPtr port = sAllDGGraphs[i]->getDGPort(data.portName[k]);
        if(port->getDGNodeName() != data.dgNode)
          boundDGNode = port->getDGNodeName();

        parameterLayoutStr[k] = boundDGNode + "." + port->getMember();
        if(opIt->second.params[k].isSliced)
          parameterLayoutStr[k] += "<>";
        parameterLayoutChar[k] = parameterLayoutStr[k].c_str();
      }

      try
      {
        binding = FabricCore::DGBinding(opIt->second.op, opIt->second.params.size(), &parameterLayoutChar[0]);
        bindings.insert(binding, data.index);
      }
      catch(FabricCore::Exception e)
      {
        bindings.remove(data.index);
        free(parameterLayoutChar);
        return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
      }

      bindings.remove(data.index+1);

      free(parameterLayoutChar);

      sAllDGGraphs[i]->requireEvaluate();
      data.valid = true;

      LoggingImpl::log(std::string("KLOperator '")+op.getName()+"' on DGNode '"+data.dgNode+"' validated.");
    }
  }
  return true;
}

unsigned int DGGraphImpl::getKLOperatorCount(const std::string & dgNodeName, std::string * errorOut)
{
  FabricCore::DGNode dgNode = getDGNode(dgNodeName);
  if(!dgNode.isValid())
  {
    LoggingImpl::reportError("DGNode '"+dgNodeName+"' does not exist.", errorOut);
    return 0;
  }

  FabricCore::DGBindingList bindings = dgNode.getBindingList();
  return (unsigned int)bindings.getCount();
}

char const * DGGraphImpl::getKLOperatorName(unsigned int index, const std::string & dgNodeName, std::string * errorOut)
{
  stringVector result;

  FabricCore::DGNode dgNode = getDGNode(dgNodeName);
  if(!dgNode.isValid())
  {
    LoggingImpl::reportError("DGNode '"+dgNodeName+"' does not exist.", errorOut);
    return "";
  }

  FabricCore::DGBindingList bindings = dgNode.getBindingList();
  if(index >= bindings.getCount())
    return "";

  FabricCore::DGBinding binding = bindings.getBinding(index);
  FabricCore::DGOperator op = binding.getOperator();
  return getPrettyDGOperatorName(op.getName());
}

stringVector DGGraphImpl::getKLOperatorNames(const std::string & dgNodeName, std::string * errorOut)
{
  stringVector result;

  FabricCore::DGNode dgNode = getDGNode(dgNodeName);
  if(!dgNode.isValid())
  {
    LoggingImpl::reportError("DGNode '"+dgNodeName+"' does not exist.", errorOut);
    return result;
  }

  FabricCore::DGBindingList bindings = dgNode.getBindingList();
  for(uint32_t i=0;i<bindings.getCount();i++)
  {
    FabricCore::DGBinding binding = bindings.getBinding(i);
    FabricCore::DGOperator op = binding.getOperator();
    std::string opName = op.getName();
    result.push_back(getPrettyDGOperatorName(opName.c_str()));
  }
  return result;
}

unsigned int DGGraphImpl::getGlobalKLOperatorCount(std::string * errorOut)
{
  unsigned int result = 0;
  for(size_t i=0;i<sAllDGGraphs.size();i++)
  {
    stringVector dgNodeNames = sAllDGGraphs[i]->getDGNodeNames();
    for(size_t j=0;j<dgNodeNames.size();j++)
    {
      result += sAllDGGraphs[i]->getKLOperatorCount(dgNodeNames[j]);
    }
  }
  return result;
}

char const * DGGraphImpl::getGlobalKLOperatorName(unsigned int index, std::string * errorOut)
{
  unsigned int offset = 0;
  for(size_t i=0;i<sAllDGGraphs.size();i++)
  {
    stringVector dgNodeNames = sAllDGGraphs[i]->getDGNodeNames();
    for(size_t j=0;j<dgNodeNames.size();j++)
    {
      size_t opCount = sAllDGGraphs[i]->getKLOperatorCount(dgNodeNames[j]);
      for(size_t k=0;k<opCount;k++)
      {
        if(offset == index)
          return sAllDGGraphs[i]->getKLOperatorName(k, dgNodeNames[j]);
        offset++;
      }
    }
  }
  return "";
}

stringVector DGGraphImpl::getGlobalKLOperatorNames(std::string * errorOut)
{
  stringVector result;

  stringMap sortedResult;
  for(size_t i=0;i<sAllDGGraphs.size();i++)
  {
    stringVector dgNodeNames = sAllDGGraphs[i]->getDGNodeNames();
    for(size_t j=0;j<dgNodeNames.size();j++)
    {
      stringVector singleResult = sAllDGGraphs[i]->getKLOperatorNames(dgNodeNames[j], errorOut);
      for(size_t k=0;k<singleResult.size();k++)
      {
        if(sortedResult.find(singleResult[k]) != sortedResult.end())
          continue;
        sortedResult.insert(stringPair(singleResult[k], ""));
      }
    }
  }

  for(stringIt it=sortedResult.begin();it!=sortedResult.end();it++)
    result.push_back(it->first);

  return result;
}

DGPortImplPtr DGGraphImpl::getDGPortFromKey(const std::string & key)
{
  stringVector parts = StringUtilityImpl::splitString(key, '.');
  for(size_t j=0;j<sAllDGGraphs.size();j++)
  {
    if(sAllDGGraphs[j]->getName() == parts[0])
    {
      DGPortImplPtr port = sAllDGGraphs[j]->getDGPort(parts[1]);
      if(port)
        return port;
    }
  }
  return DGPortImplPtr();
}

void DGGraphImpl::setMemberPersistence(const std::string &name, bool persistence){
  mMemberPersistenceOverrides[name] = persistence ? 1 : 0;
}

bool DGGraphImpl::memberPersistence(const std::string &name, const std::string &type, bool * requiresStorage)
{
  bool persistence = false;
  DGPortImplPtr port = getDGPort(name);
  if(port)
    persistence = !port->isArray() && (port->isShallow() || strcmp(port->getDataType(), "String") == 0) && !port->isObject() && !port->isInterface();    
  if(mMemberPersistenceOverrides.find(name) != mMemberPersistenceOverrides.end()){
    if(requiresStorage)
      *requiresStorage = persistence != (mMemberPersistenceOverrides[name] != 0);
    persistence = mMemberPersistenceOverrides[name] != 0;
  } else if(requiresStorage)
    *requiresStorage = false;
  return persistence;
}

void DGGraphImpl::getDGPortInfo(FabricCore::Variant & portInfo, FabricCore::RTVal persistenceContextRT, bool forPersistence)
{
  portInfo = FabricCore::Variant::CreateArray();
  for(DGPortIt it = mDGPorts.begin(); it != mDGPorts.end(); it++)
  {
    FabricCore::Variant valueVar = FabricCore::Variant::CreateDict();
    valueVar.setDictValue("name", FabricCore::Variant::CreateString(it->second->getName()));

    if(!forPersistence)
      valueVar.setDictValue("graph", FabricCore::Variant::CreateString(getName_cstr()));

    // don't persist the node if it is the default node
    if(mDGNodes.size() > 1 || !forPersistence)
      valueVar.setDictValue("node", FabricCore::Variant::CreateString(it->second->getDGNodeName()));

    std::string dataType = it->second->getDataType();
    if(it->second->isArray())
      dataType += "[]";
    valueVar.setDictValue("type", FabricCore::Variant::CreateString(dataType.c_str()));

    // don't persist the auto init defaults
    if(!it->second->doesAutoInitObjects())
      valueVar.setDictValue("autoInitObjects", FabricCore::Variant::CreateBoolean(false));

    // don't persist the member if it matches the port name
    if(std::string(it->second->getMember()) != it->second->getName())
      valueVar.setDictValue("member", FabricCore::Variant::CreateString(it->second->getMember()));

    if(it->second->getMode() == DGPortImpl::Mode_IN)
    {
      // don't persist in, it's the default
      // valueVar.setDictValue("mode", FabricCore::Variant::CreateString("in"));
    }
    else if(it->second->getMode() == DGPortImpl::Mode_OUT)
      valueVar.setDictValue("mode", FabricCore::Variant::CreateString("out"));
    else
      valueVar.setDictValue("mode", FabricCore::Variant::CreateString("io"));

    FabricCore::Variant options = it->second->getAllOptions();
    if(options.isDict())
      valueVar.setDictValue("options", options);

    // only save non-standard persistence flags
    bool requiresStorage = false;
    bool persistence = memberPersistence(it->second->getName(), dataType, &requiresStorage);
    if(requiresStorage)
      valueVar.setDictValue("persistence", FabricCore::Variant::CreateBoolean(persistence));

    FabricCore::Variant defaultDataVar;
    if(persistence)
    {
      if(!it->second->isObject() && !it->second->isInterface())
      {
        FabricCore::Variant defaultDataVar = it->second->getVariant();
        valueVar.setDictValue("default", defaultDataVar.getJSONEncoding());
      }

      if(persistenceContextRT.isValid())
      {
        FabricCore::RTVal rtVal = it->second->getRTVal();
        if(rtVal.isValid())
        {
          if(rtVal.isObject())
          {
            if(!rtVal.isNullObject())
            {
              FabricCore::RTVal objectRtVal = FabricSplice::constructRTVal("Object", 1, &rtVal);
              if(objectRtVal.isValid())
              {
                FabricCore::RTVal persistable = FabricSplice::constructInterfaceRTVal("Persistable", objectRtVal);
                if(!persistable.isNullObject())
                {
                  try
                  {
                    FabricCore::RTVal stringRTVal = persistable.callMethod("String", "saveDataToString", 1, &persistenceContextRT);
                    valueVar.setDictValue("persistenceData", FabricCore::Variant::CreateString(stringRTVal.getStringCString(), stringRTVal.getStringLength()));
                  }
                  catch(FabricCore::Exception e)
                  {
                    //ignore errors, assume we couldn't cast.
                    //LoggingImpl::reportError(e.getDesc_cstr());
                  }
                }
              }
            }
          }
        }
      }
    }

    portInfo.arrayAppend(valueVar);
  }
}

FabricCore::Variant DGGraphImpl::getPersistenceDataDict(const PersistenceInfo * info)
{
  mIsPersisting = true;

  FabricCore::Variant dataVar = FabricCore::Variant::CreateDict();
  dataVar.setDictValue("version", FabricCore::Variant::CreateSInt32(SPLICE_API_VERSION).getJSONEncoding());

  if(mMetaData.length() > 0)
    dataVar.setDictValue("metaData", FabricCore::Variant::CreateString(mMetaData.c_str()));

  if ( mEvaluateShared )
    dataVar.setDictValue(
      "evaluateShared",
      FabricCore::Variant::CreateBoolean( mEvaluateShared )
      );

  FabricCore::RTVal persistenceContextRT = FabricSplice::constructObjectRTVal("PersistenceContext");
  persistenceContextRT = persistenceContextRT.callMethod("PersistenceContext", "getInstance", 0, 0);
  if(info != NULL)
  {
    FabricCore::Variant infoVar = FabricCore::Variant::CreateDict();
    if(!info->hostAppName.isNull())
      infoVar.setDictValue("hostAppName", info->hostAppName);
    if(!info->hostAppVersion.isNull())
      infoVar.setDictValue("hostAppVersion", info->hostAppVersion);
    dataVar.setDictValue("info", infoVar);


    persistenceContextRT.setMember("hostName", FabricSplice::constructStringRTVal(info->hostAppName.getStringData()));
    persistenceContextRT.setMember("hostVersion", FabricSplice::constructStringRTVal(info->hostAppVersion.getStringData()));
    persistenceContextRT.setMember("filePath", FabricSplice::constructStringRTVal(info->filePath.getStringData()));
  }

  // only persist the splice base path, if applicable
  if(mIsReferenced)
  {
    dataVar.setDictValue("spliceFilePath", FabricCore::Variant::CreateString(mFilePath.c_str()));
  }
  else
  {
    FabricCore::Variant dgNodeListVar = FabricCore::Variant::CreateArray();
    FabricCore::Variant portData = FabricCore::Variant::CreateDict();
    for(DGNodeIt it = mDGNodes.begin(); it != mDGNodes.end(); it++)
    {
      FabricCore::Variant dgNodeVar = FabricCore::Variant::CreateDict();

      // only save non-default names
      if(it->first != "DGNode")
        dgNodeVar.setDictValue("name", FabricCore::Variant::CreateString(it->first.c_str()));

      FabricCore::DGNode node = it->second.node;

      FabricCore::Variant dependenciesVar = node.getDependencies_Variant();
      if(dependenciesVar.isArray())
      {
        if(dependenciesVar.getArraySize() > 0)
          dgNodeVar.setDictValue("dependencies", dependenciesVar);
      }

      FabricCore::Variant bindingListVar = FabricCore::Variant::CreateArray();

      FabricCore::DGBindingList bindings = node.getBindingList();
      for(uint32_t i=0;i<bindings.getCount();i++)
      {
        FabricCore::DGBinding binding = bindings.getBinding(i);
        FabricCore::DGOperator op = binding.getOperator();

        FabricCore::Variant bindingVar = FabricCore::Variant::CreateDict();
        FabricCore::Variant opVar = FabricCore::Variant::CreateDict();

        std::string opName = getPrettyDGOperatorName(op.getName());

        opVar.setDictValue("name", FabricCore::Variant::CreateString(opName.c_str()));

        // only save entry points which differ from the op name
        if(opName != op.getEntryPoint())
          opVar.setDictValue("entry", FabricCore::Variant::CreateString(op.getEntryPoint()));

        // either store the filename with the code, or just the filename.
        // this should be based on a user decision, if the kl file is based 
        // on an external file, or not.
        stringIt fileNameIt = mKLOperatorFileNames.find(op.getName());
        if(fileNameIt == mKLOperatorFileNames.end())
        {
          // check if we have operator source code in the DCC UI somewhere
          std::string klCode;
          if(sGetOperatorSourceCodeFunc)
          {
            const char * klCodeCStr = (*sGetOperatorSourceCodeFunc)(getName().c_str(), opName.c_str());
            if(klCodeCStr)
              klCode = klCodeCStr;
          }
          if(klCode.length() == 0)
          {
            klCode = getKLOperatorSourceCode(opName);
          }
          opVar.setDictValue("kl", FabricCore::Variant::CreateString(klCode.c_str()));
        }
        else
        {
          opVar.setDictValue("filename", FabricCore::Variant::CreateString(fileNameIt->second.c_str()));
        }

        if(i < it->second.opPortMaps.size())
        {
          bool hasContent = !FabricCore::Variant::DictIter(it->second.opPortMaps[i]).isDone();
          if(hasContent)
            opVar.setDictValue("portmap", it->second.opPortMaps[i]);
        }

        // todo: we might want to store the parameter layout at a some point
        bindingVar.setDictValue("operator", opVar);
        bindingListVar.arrayAppend(bindingVar);
      }
      dgNodeVar.setDictValue("bindings", bindingListVar);

      dgNodeListVar.arrayAppend(dgNodeVar);
    }
    dataVar.setDictValue("nodes", dgNodeListVar);

    FabricCore::Variant extensionListVar = FabricCore::Variant::CreateArray();
    for(stringIt it = mLoadedExtensions.begin(); it != mLoadedExtensions.end(); it++)
      extensionListVar.arrayAppend(FabricCore::Variant::CreateString(it->first.c_str()));
    if(mLoadedExtensions.size() > 0)
      dataVar.setDictValue("extensions", extensionListVar);

    FabricCore::Variant portInfo;
    getDGPortInfo(portInfo, persistenceContextRT, true);
    dataVar.setDictValue("ports", portInfo);
  }

  mIsPersisting = false;

  return dataVar;
}

std::string DGGraphImpl::getPersistenceDataJSON(const PersistenceInfo * info)
{
  return getPersistenceDataDict(info).getJSONEncoding().getStringData();
}

bool DGGraphImpl::setFromPersistenceDataDict(
  DGGraphImplPtr thisGraph,
  const FabricCore::Variant & dict, 
  PersistenceInfo * info,
  const char * baseFilePath,
  std::string * errorOut
) {

  if(!thisGraph)
    return LoggingImpl::reportError("Parameter thisGraph is not valid.", errorOut);
  if(thisGraph.get() != this)
    return LoggingImpl::reportError("Parameter thisGraph does not refer to this graph.", errorOut);

  FabricCore::Variant dataVar = dict;
  const FabricCore::Variant * versionVar = dataVar.getDictValue("version");
  if(!versionVar)
    return LoggingImpl::reportError("JSON data is corrupt, no 'version' element.", errorOut);
  const FabricCore::Variant * metaDataVar = dataVar.getDictValue("metaData");
  if(metaDataVar)
    if(metaDataVar->isString())
      mMetaData = metaDataVar->getStringData();
  if(mMetaData.length() > 0)
    dataVar.setDictValue("metaData", FabricCore::Variant::CreateString(mMetaData.c_str()));

  FabricCore::Variant const *evaluateSharedVar =
    dataVar.getDictValue("evaluateShared");
  if ( evaluateSharedVar && evaluateSharedVar->isBoolean() )
    mEvaluateShared = evaluateSharedVar->getBoolean();

  // check if this is a referenced splice file
  const FabricCore::Variant * spliceFilePathVar = dataVar.getDictValue("spliceFilePath");
  if(spliceFilePathVar)
  {
    std::string spliceFilePath = spliceFilePathVar->getStringData();
    return loadFromFile(thisGraph, spliceFilePath, info, true, errorOut);
  }

  const FabricCore::Variant * nodesVar = dataVar.getDictValue("nodes");
  if(!nodesVar)
    return LoggingImpl::reportError("JSON data is corrupt, no 'nodes' element.", errorOut);
  const FabricCore::Variant * valuesVar = dataVar.getDictValue("ports");
  if(!valuesVar)
    return LoggingImpl::reportError("JSON data is corrupt, no 'ports' element.", errorOut);
  const FabricCore::Variant * extensionsVar = dataVar.getDictValue("extensions");

  clear(errorOut);

  std::string version = versionVar->getStringData();

  if(info)
  {
    const FabricCore::Variant * infoVar = dataVar.getDictValue("info");
    if(infoVar)
    {
      if(infoVar->isDict())
      {
        // [phtaylor 16/07/2014] Removed this code. I can't figure out who is recieving this data. 
        // My understanding is that it ould be used to show that a splice preset was saved from Maya 
        // and loaded into Softimage. We could generate a message saying "Loading data saved from X application"
        // info->hostAppName = *infoVar->getDictValue("hostAppName");
        // info->hostAppVersion = *infoVar->getDictValue("hostAppVersion");
      }
    }
  }

  FabricCore::RTVal persistenceContextRT = FabricSplice::constructObjectRTVal("PersistenceContext");
  persistenceContextRT = persistenceContextRT.callMethod("PersistenceContext", "getInstance", 0, 0);
  if(info != NULL)
  {
    // [phtaylor 16/07/2014] Once again, I don't know why we are writing out data during a load operation. 
    // The dict (and dataVar) should be const during the load. 
    // Note: by removing this code, I eliminated corruption while loading Scenes in Softimage. 
    // the corruption caused the extensionsVar to become garbage. (I don't know why this didn't happen in Maya.)
    // FabricCore::Variant infoVar = FabricCore::Variant::CreateDict();
    // if(!info->hostAppName.isNull())
    //   infoVar.setDictValue("hostAppName", info->hostAppName);
    // if(!info->hostAppVersion.isNull())
    //   infoVar.setDictValue("hostAppVersion", info->hostAppVersion);
    // dataVar.setDictValue("info", infoVar);

    persistenceContextRT.setMember("hostName", FabricSplice::constructStringRTVal(info->hostAppName.getStringData()));
    persistenceContextRT.setMember("hostVersion", FabricSplice::constructStringRTVal(info->hostAppVersion.getStringData()));
    persistenceContextRT.setMember("filePath", FabricSplice::constructStringRTVal(info->filePath.getStringData()));
  }


  if(extensionsVar && extensionsVar->isArray())
  {
    for(uint32_t i=0;i<extensionsVar->getArraySize();i++)
    {
      const FabricCore::Variant * extensionVar = extensionsVar->getArrayElement(i);
      if(!extensionVar)
        return LoggingImpl::reportError("JSON data is corrupt, 'extensions' element doesn't contain elements.", errorOut);
      std::string extension = extensionVar->getStringData();
      if(mLoadedExtensions.find(extension) != mLoadedExtensions.end())
        continue;
      if(!loadExtension(extension, errorOut))
        return false;
      mLoadedExtensions.insert(stringPair(extension, extension));
    }
  }

  for(uint32_t i=0;i<nodesVar->getArraySize();i++)
  {
    const FabricCore::Variant * nodeVar = nodesVar->getArrayElement(i);
    const FabricCore::Variant * nodeNameVar = nodeVar->getDictValue("name");
    std::string dgNodeName = "DGNode";
    if(nodeNameVar)
      dgNodeName = nodeNameVar->getStringData();

    if(!constructDGNode(dgNodeName, errorOut))
      return false;
  }

  for(uint32_t i=0;i<valuesVar->getArraySize();i++)
  {
    const FabricCore::Variant * valueVar = valuesVar->getArrayElement(i);
    const FabricCore::Variant * valueNameVar = valueVar->getDictValue("name");
    if(!valueNameVar)
      return LoggingImpl::reportError("JSON data is corrupt, 'valueinterface' element doesn't contain 'name' element.", errorOut);
    const FabricCore::Variant * valueTypeVar = valueVar->getDictValue("type");
    if(!valueTypeVar)
      return LoggingImpl::reportError("JSON data is corrupt, 'valueinterface' element doesn't contain 'type' element.", errorOut);

    std::string dgNodeName = "DGNode";
    const FabricCore::Variant * valueDGNodeVar = valueVar->getDictValue("node");
    if(valueDGNodeVar)
      dgNodeName = valueDGNodeVar->getStringData();

    std::string memberName = valueNameVar->getStringData();
    const FabricCore::Variant * valueMemberVar = valueVar->getDictValue("member");
    if(valueMemberVar)
      memberName = valueMemberVar->getStringData();

    std::string modeStr = "in";
    const FabricCore::Variant * valueModeVar = valueVar->getDictValue("mode");
    if(valueModeVar)
      modeStr = valueModeVar->getStringData();

    bool autoInitObjects = true;
    const FabricCore::Variant * autoInitObjectsVar = valueVar->getDictValue("autoInitObjects");
    if(autoInitObjectsVar)
      if(autoInitObjectsVar->isBoolean())
        autoInitObjects = autoInitObjectsVar->getBoolean();

    const FabricCore::Variant * optionsVar = valueVar->getDictValue("options");

    const FabricCore::Variant * defaultValueVar = valueVar->getDictValue("default");
    const FabricCore::Variant * valuePersistenceVar = valueVar->getDictValue("persistence");
    const FabricCore::Variant * valuePersistenceDataVar = valueVar->getDictValue("persistenceData");

    // backwards compatibility for <= 1.12
    if(!defaultValueVar && !valuePersistenceVar && !valuePersistenceDataVar)
    {
      for(uint32_t j=0;j<nodesVar->getArraySize();j++)
      {
        const FabricCore::Variant * nodeVar = nodesVar->getArrayElement(j);
        std::string valueDgNodeName = "DGNode";
        const FabricCore::Variant * nodeNameVar = nodeVar->getDictValue("name");
        if(nodeNameVar)
          valueDgNodeName = nodeNameVar->getStringData();
        if(dgNodeName == valueDgNodeName)
        {
          const FabricCore::Variant * membersVar = nodeVar->getDictValue("members");
          if(membersVar)
          {
            for(uint32_t k=0;k<membersVar->getArraySize();k++)
            {
              const FabricCore::Variant * memberVar = membersVar->getArrayElement(k);
              const FabricCore::Variant * memberNameVar = memberVar->getDictValue("name");
              std::string memberName = memberNameVar->getStringData();
              if(memberName == valueNameVar->getStringData())
              {
                defaultValueVar = memberVar->getDictValue("default");
                valuePersistenceVar = memberVar->getDictValue("persistence");
                valuePersistenceDataVar = memberVar->getDictValue("persistenceData");
                break;
              }
            }
          }
          break;
        }
      }
    }

    FabricCore::Variant defaultValue;
    if(defaultValueVar)
    {
      std::string defaultValueStr = defaultValueVar->getStringData();
      if(defaultValueStr.length() > 0)
        defaultValue = FabricCore::Variant::CreateFromJSON(defaultValueStr.c_str());
    }

    // try a second time without a default value
    bool addMemberResult = addDGNodeMember(
      memberName.c_str(), 
      valueTypeVar->getStringData(), 
      defaultValue, 
      dgNodeName.c_str(),
      "",
      autoInitObjects,
      errorOut
    );
    if(!addMemberResult)
    {
      addMemberResult = addDGNodeMember(
        memberName.c_str(), 
        valueTypeVar->getStringData(), 
        FabricCore::Variant(), 
        dgNodeName.c_str(),
        "",
        autoInitObjects,
        errorOut
      );
      if(addMemberResult)
        LoggingImpl::clearError();
    }
    if(!addMemberResult)
      return false;

    if(valuePersistenceVar)
    {
      bool persistence = valuePersistenceVar->getBoolean();
      setMemberPersistence(memberName.c_str(), persistence);
    }
    else
    {
      // backwards compatibility from 1.0.2-beta
      const FabricCore::Variant * shouldPersistVar = valueVar->getDictValue("shouldPersist");
      if(shouldPersistVar)
      {
        bool shouldPersist = shouldPersistVar->getBoolean();
        setMemberPersistence(memberName.c_str(), shouldPersist);
      }
    }

    if(valuePersistenceVar && valuePersistenceDataVar)
    {
      if(valuePersistenceDataVar->isString())
      {
        FabricCore::DGNode dgNode = getDGNode(dgNodeName.c_str());
        if(dgNode.isValid()){
          FabricCore::RTVal memberRTVal = dgNode.getMemberSliceValue(memberName.c_str(), 0);
          if(memberRTVal.isValid() && !memberRTVal.isNullObject())
          {
            FabricCore::RTVal objectRtVal = FabricSplice::constructRTVal("Object", 1, &memberRTVal);
            if(objectRtVal.isValid())
            {
              FabricCore::RTVal persistable = FabricSplice::constructInterfaceRTVal("Persistable", objectRtVal);
              if(!persistable.isNullObject())
              {
                try
                {
                  FabricCore::RTVal args[2];
                  args[0] = persistenceContextRT;
                  args[1] = FabricSplice::constructStringRTVal(valuePersistenceDataVar->getStringData());
                  persistable.callMethod("", "loadDataFromString", 2, &args[0]);
                }
                catch(FabricCore::Exception e)
                {
                  LoggingImpl::reportError(e.getDesc_cstr());
                }
              }
            }
          }
        }
      }
    }

    DGPortImpl::Mode mode = DGPortImpl::Mode_IO;
    if(modeStr == "in")
      mode = DGPortImpl::Mode_IN;
    else if(modeStr == "out")
      mode = DGPortImpl::Mode_OUT;

    if(!addDGPort(
      thisGraph,
      valueNameVar->getStringData(), 
      memberName.c_str(), 
      mode,
      dgNodeName.c_str(), 
      autoInitObjects,
      errorOut
    )) return false;

    if(optionsVar)
    {
      DGPortImplPtr port = getDGPort(valueNameVar->getStringData());
      FabricCore::Variant::DictIter optionsIter(*optionsVar);
      for(;!optionsIter.isDone();optionsIter.next())
      {
        const FabricCore::Variant * optionName = optionsIter.getKey();
        const FabricCore::Variant * optionValue = optionsIter.getValue();
        port->setOption(optionName->getStringData(), *optionValue);
      }
    }
  }

  for(uint32_t i=0;i<nodesVar->getArraySize();i++)
  {
    const FabricCore::Variant * nodeVar = nodesVar->getArrayElement(i);
    const FabricCore::Variant * nodeNameVar = nodeVar->getDictValue("name");
    const FabricCore::Variant * bindingsVar = nodeVar->getDictValue("bindings");
    if(!bindingsVar)
      return LoggingImpl::reportError("JSON data is corrupt, 'node' element doesn't contain 'bindings' element.", errorOut);

    std::string dgNodeName = "DGNode";
    if(nodeNameVar)
      dgNodeName = nodeNameVar->getStringData();

    const FabricCore::Variant * dependenciesVar = nodeVar->getDictValue("dependencies");
    if(dependenciesVar)
    {
      if(dependenciesVar->isDict())
      {
        for(FabricCore::Variant::DictIter keyIter(*dependenciesVar); !keyIter.isDone(); keyIter.next())
        {
          std::string key = keyIter.getKey()->getStringData();
          if(!setDGNodeDependency(dgNodeName, key))
            return false;
        }
      }
    }

    for(uint32_t i=0;i<bindingsVar->getArraySize();i++)
    {
      const FabricCore::Variant * bindingVar = bindingsVar->getArrayElement(i);
      const FabricCore::Variant * operatorVar = bindingVar->getDictValue("operator");
      if(!operatorVar)
        return LoggingImpl::reportError("JSON data is corrupt, 'binding' element doesn't contain 'operator' element.", errorOut);


      const FabricCore::Variant * operatorNameVar = operatorVar->getDictValue("name");
      if(!operatorNameVar)
        return LoggingImpl::reportError("JSON data is corrupt, 'operator' element doesn't contain 'name' element.", errorOut);
      std::string opName = operatorNameVar->getStringData();

      std::string fileNameStr = opName+".kl";
      const FabricCore::Variant * operatorFileNameVar = operatorVar->getDictValue("filename");
      if(operatorFileNameVar)
        fileNameStr = operatorFileNameVar->getStringData();

      const FabricCore::Variant * operatorEntryVar = operatorVar->getDictValue("entry");
      std::string entry = opName;
      if(operatorEntryVar)
        if(operatorEntryVar->isString())
          entry = operatorEntryVar->getStringData();

      std::string klCode = "operator "+entry+"() {}";
      const FabricCore::Variant * operatorKLVar = operatorVar->getDictValue("kl");
      bool fileNeedsToExist = true;
      if(operatorKLVar)
      {
        klCode = operatorKLVar->getStringData();;
        if (!klCode.empty())
          fileNeedsToExist = false;
      }

      const FabricCore::Variant * opPortMapVar = operatorVar->getDictValue("portmap");

      std::string resolvedFilePath = resolveEnvironmentVariables(fileNameStr);
      std::ifstream file(resolvedFilePath.c_str());
      if(!file && baseFilePath)
      {
        resolvedFilePath = resolveRelativePath(baseFilePath, resolvedFilePath);
        file.open(resolvedFilePath.c_str());
      }

      bool isFileBased = false;
      if(file)
      {
        std::stringstream buffer;
        buffer << file.rdbuf();
        klCode = buffer.str();
        isFileBased = true;
      }
      else if(fileNeedsToExist)
      {
        std::string errorString = "The KL file '"+fileNameStr+"' for KL operator '"+opName+"' cannot be found.";
        throw FabricCore::Exception(errorString.c_str());
      }

      if(!constructKLOperator(
        opName.c_str(),
        klCode.c_str(), 
        entry.c_str(),
        dgNodeName,
        (opPortMapVar == NULL) ? FabricCore::Variant::CreateDict() : *opPortMapVar,
        errorOut
      )) {

        // continue loading the other operators
        if(LoggingImpl::hasError())
        {
          LoggingImpl::logError(LoggingImpl::getError());
          LoggingImpl::clearError();
        }
        continue;
      }

      if(isFileBased)
      {
        std::string realOpName = getRealDGOperatorName(opName.c_str());
        if(mKLOperatorFileNames.find(realOpName) == mKLOperatorFileNames.end())
          mKLOperatorFileNames.insert(stringPair(realOpName, fileNameStr));
        else
          mKLOperatorFileNames.find(realOpName)->second = fileNameStr;
      }

      DGOperatorIt opIt = sDGOperators.find(getRealDGOperatorName(opName.c_str()));
      if(opIt != sDGOperators.end())
        opIt->second.op.setFilename(fileNameStr.c_str());
    }
  }

  requireEvaluate();
  return true;
}

bool DGGraphImpl::setFromPersistenceDataJSON(
  DGGraphImplPtr thisGraph, 
  const std::string & json, 
  PersistenceInfo * info,
  const char * baseFilePath,
  std::string * errorOut
) {
  return setFromPersistenceDataDict(thisGraph, FabricCore::Variant::CreateFromJSON(json.c_str()), info, baseFilePath, errorOut);
}

bool DGGraphImpl::saveToFile(const std::string & filePath, const PersistenceInfo * info, std::string * errorOut)
{
  if(getKLOperatorNames().size() == 0)
    return LoggingImpl::reportError("Splice does not contain any operators.", errorOut);

  std::string json = getPersistenceDataJSON(info);

  FILE * file = fopen(filePath.c_str(), "wb");
  if(!file)
    return LoggingImpl::reportError("Invalid filePath '"+filePath+"'", errorOut);

  fwrite(json.c_str(), json.length(), 1, file);

  fclose(file);

  LoggingImpl::log("Saved graph '"+getName()+"' to "+filePath);
  return true;
}

bool DGGraphImpl::loadFromFile(
  DGGraphImplPtr thisGraph,
  const std::string & filePath, 
  PersistenceInfo * info,
  bool asReferenced,
  std::string * errorOut
) {
  if(!thisGraph)
    return LoggingImpl::reportError("Parameter thisGraph is not valid.", errorOut);
  if(thisGraph.get() != this)
    return LoggingImpl::reportError("Parameter thisGraph does not refer to this graph.", errorOut);

  std::string resolvedFilePath = resolveEnvironmentVariables(filePath);
  FILE * file = fopen(resolvedFilePath.c_str(), "rb");
  
  // try using search path
  if(!file)
  {
    resolvedFilePath = filePath;
    file = findFileInSearchPath(resolvedFilePath);
  }

  if(!file)
    return LoggingImpl::reportError("Invalid filePath '"+filePath+"'", errorOut);

  fseek( file, 0, SEEK_END );
  long fileSize = ftell( file );
  rewind( file );

  char * buffer = (char*) malloc(fileSize + 1);
  buffer[fileSize] = '\0';

  size_t readBytes = fread(buffer, 1, fileSize, file);
  assert(readBytes == size_t(fileSize));
  (void)readBytes;

  fclose(file);

  std::string json = buffer;
  free(buffer);

  mIsReferenced = asReferenced;
  mFilePath = filePath;

  if(!setFromPersistenceDataJSON(thisGraph, json, info, resolvedFilePath.c_str(), errorOut))
    return false;
  LoggingImpl::log("Loaded graph '"+getName()+"' from "+filePath);

  return true;
}

bool DGGraphImpl::reloadFromFile(DGGraphImplPtr thisGraph, PersistenceInfo * info, std::string * errorOut)
{
  if(!isReferenced())
  {
    LoggingImpl::logError("The DGGraph '"+getName()+"' is not referenced, this it cannot be reloaded.");
    return false;
  }

  clear();
  return loadFromFile(thisGraph, mFilePath, info, true, errorOut);
}

bool DGGraphImpl::isReferenced()
{
  return mIsReferenced;
}

const char * DGGraphImpl::getReferencedFilePath()
{
  return mFilePath.c_str();
}

bool DGGraphImpl::requireEvaluate()
{
  if(mRequiresEval)
    return false;

  for(DGNodeIt nodeIt = mDGNodes.begin(); nodeIt != mDGNodes.end(); nodeIt++)
    nodeIt->second.node.setDirty();

  mRequiresEval = true;
  return true;
}

char const * DGGraphImpl::getRealDGOperatorName(const char * name) const
{
  stringConstIt it = mDGOperatorNameMap.find(name);
  if(it == mDGOperatorNameMap.end())
    return name;
  return it->second.c_str();
}

char const * DGGraphImpl::getPrettyDGOperatorName(const char * name) const
{
  for(stringConstIt it = mDGOperatorNameMap.begin(); it != mDGOperatorNameMap.end(); it++)
  {
    if(name != it->second)
      continue;
    return it->first.c_str();
  }
  return name;
}

std::string DGGraphImpl::resolveRelativePath(const std::string & baseFile, const std::string text)
{
  if(baseFile.length() == 0)
    return text;

  size_t lastSlashPos = baseFile.rfind('/');
  size_t lastBackSlashPos = baseFile.rfind('\\');
  if(lastSlashPos != std::string::npos && lastBackSlashPos != std::string::npos)
    lastSlashPos = lastSlashPos > lastBackSlashPos ? lastSlashPos : lastBackSlashPos;

  if(lastSlashPos != std::string::npos)
    return baseFile.substr(0, lastSlashPos + 1) + text;

  return text;
}

std::string DGGraphImpl::resolveEnvironmentVariables(const std::string text)
{
  std::string output;
  if(text.length() > 0)
  {
    for(unsigned int i=0;i<text.length()-1;i++)
    {
      if(text[i] == '$' && text[i+1] == '{')
      {
        size_t closePos = text.find('}', i);
        if(closePos != std::string::npos)
        {
          std::string envVarName = text.substr(i+2, closePos - i - 2);
          const char * envVarValue = getenv(envVarName.c_str());
          if(envVarValue != NULL)
          {
            output += envVarValue;
            i = (unsigned int)closePos;
            continue;
          }
        }
      }
      output += text[i];
    }

    if(text[text.length()-1] != '}')
      output += text[text.length()-1];
  }
  return output;
}

FILE* DGGraphImpl::findFileInSearchPath(std::string& resolvedFilePath)
{
  FILE* file = NULL;
  std::string userPath = resolvedFilePath;
  const char * searchPaths = getenv("FABRIC_NODES_PATH");

  if(searchPaths != NULL)
  {
    std::vector<std::string> paths;
#ifdef _WIN32    
    boost::split(paths, searchPaths, boost::is_any_of(";,"));
#else
    boost::split(paths, searchPaths, boost::is_any_of(":;,"));
#endif
    for(size_t i = 0; i < paths.size(); i++)
    {
      std::string path = resolveEnvironmentVariables(paths[i]);
      resolvedFilePath = FTL::PathJoin(path, userPath);
      // LoggingImpl::log(("Testing filepath: "+resolvedFilePath).c_str());
      file = fopen(resolvedFilePath.c_str(), "rb");
      if(file)
      {
        LoggingImpl::log(("Resolved to filepath: "+resolvedFilePath).c_str());
        break;
      }
    }
  }
  return file;
}
