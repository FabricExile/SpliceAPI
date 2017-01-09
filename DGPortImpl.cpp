// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#include "DGPortImpl.h"
#include "SceneManagementImpl.h"

using namespace FabricSpliceImpl;

DGPortImpl::DGPortImpl(
  DGGraphImplPtr graph,
  const std::string & name, 
  const std::string & member, 
  FabricCore::DGNode dgNode, 
  const std::string & dgNodeName, 
  Mode mode,
  uint32_t dataSize,
  bool shallow,
  bool autoInitObjects
) {
  const FabricCore::Client * client = DGGraphImpl::getClient();

  mGraph = DGGraphImplWeakPtr(graph);
  mGraphName = graph->getName();
  setName(name);
  mMember = member;
  mDGNode = dgNode;
  mDGNodeName = dgNodeName;
  mMode = mode;
  // mManipulatable = -1;
  mAutoInitObjects = autoInitObjects;

  mKey = StringUtilityImpl::replaceString(mGraphName, '.', '_');
  mKey += "." + StringUtilityImpl::replaceString(getName(), '.', '_');

  mIsShallow = shallow;
  mDataType = graph->getDGNodeMemberDataType(mMember, dgNodeName);
  mIsArray = StringUtilityImpl::endsWith(mDataType, "[]");
  if(mIsArray)
    mDataType = mDataType.substr(0, mDataType.length() - 2);
  mDataSize = dataSize;

  try
  {
    mIsStruct = FabricCore::GetRegisteredTypeIsStruct(*client, mDataType.c_str());
  }
  catch(FabricCore::Exception e)
  {
    mIsStruct = false;
  }
  try
  {
    mIsObject = FabricCore::GetRegisteredTypeIsObject(*client, mDataType.c_str());
  }
  catch(FabricCore::Exception e)
  {
    mIsObject = false;
  }
  try
  {
    mIsInterface = FabricCore::GetRegisteredTypeIsInterface(*client, mDataType.c_str());
  }
  catch(FabricCore::Exception e)
  {
    mIsInterface = false;
  }

  // if the DGPort uses a KL object, let's initiate it
  if(!mIsArray && isObject() && doesAutoInitObjects()) {
    try
    {
      FabricCore::RTVal rt = getRTVal();
      if(rt.isNullObject())
      {
        rt = FabricCore::RTVal::Create(*client, mDataType.c_str(), 0, 0);
        setRTVal( rt );
        LoggingImpl::log(("DGPort '"+getName()+"' on Node '"+mGraphName+"' initiated "+mDataType+" reference.").c_str());
      }
    }
    catch(FabricCore::Exception e)
    {
      std::string message = e.getDesc_cstr();
      LoggingImpl::reportError((message+" - "+mDataType+" not initiated.").c_str());
      LoggingImpl::clearError();
    }
  }
}

DGPortImpl::~DGPortImpl()
{
  LoggingImpl::log("DGPort '"+getName()+"' on Node '"+mGraphName+"' destroyed.");
}

DGGraphImplPtr DGPortImpl::getDGGraph()
{
  if(mGraph.expired())
    return DGGraphImplPtr();
  return DGGraphImplPtr(mGraph);
}

uint32_t DGPortImpl::getSliceCount(std::string * errorOut)
{
  DGGraphImplPtr node = getDGGraph();
  if(!node)
  {
    LoggingImpl::reportError("DGPortImpl::getSliceCount, Node '"+mGraphName+"' already destroyed.");
    return 0;
  }
  return mDGNode.getSize();
}

bool DGPortImpl::setSliceCount(uint32_t count, std::string * errorOut)
{
  if(mMode == Mode_OUT)
    return LoggingImpl::reportError("Cannot set a slice count on an output DGPort.", errorOut);
  mDGNode.setSize(count);
  DGGraphImplPtr node = getDGGraph();
  if(!node)
    return LoggingImpl::reportError("DGPortImpl::setSliceCount, Node '"+mGraphName+"' already destroyed.");
  node->requireEvaluate();
  return true;
}

FabricCore::Variant DGPortImpl::getVariant(uint32_t slice, std::string * errorOut)
{
  if(slice > getSliceCount())
  {
    LoggingImpl::reportError("Slice out of bounds.", errorOut);
    return FabricCore::Variant();
  }
  DGGraphImplPtr node = getDGGraph();
  if(!node)
  {
    LoggingImpl::reportError("DGPortImpl::getVariant, Node '"+mGraphName+"' already destroyed.");
    return FabricCore::Variant();
  }

  if(mMode != Mode_IN)
    if(!node->evaluate(mDGNode, errorOut))
      return FabricCore::Variant();

  try
  {
    FabricCore::Variant result = mDGNode.getMemberSliceData_Variant(mMember.c_str(), slice);
    return result;
  }
  catch(FabricCore::Exception e)
  {
    LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }
  return FabricCore::Variant();
}

bool DGPortImpl::setVariant(FabricCore::Variant value, uint32_t slice, std::string * errorOut)
{
  if(mMode == Mode_OUT)
    return LoggingImpl::reportError("Cannot set data on an output DGPort.", errorOut);
  if(slice > mDGNode.getSize())
    return LoggingImpl::reportError("Slice out of bounds.", errorOut);

  // add missing dictionary members using the previous value
  if(value.isDict()) {
    FabricCore::Variant prevValue = getVariant();
    for(FabricCore::Variant::DictIter keyIter(prevValue); !keyIter.isDone(); keyIter.next())
    {
      std::string key = keyIter.getKey()->getStringData();
      const FabricCore::Variant * newValue = value.getDictValue(key.c_str());
      if(newValue == NULL) {
        const FabricCore::Variant * oldValue = keyIter.getValue();
        value.setDictValue(key.c_str(), *oldValue);
      }
    }
  }

  try
  {
    mDGNode.setMemberSliceData_Variant(mMember.c_str(), slice, value);
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }
  DGGraphImplPtr node = getDGGraph();
  if(!node)
    return LoggingImpl::reportError("DGPortImpl::setVariant, Node '"+mGraphName+"' already destroyed.");
  node->requireEvaluate();
  return true;
}

std::string DGPortImpl::getJSON(uint32_t slice, std::string * errorOut)
{
  if(mMode == Mode_IN)
  {
    LoggingImpl::reportError("Cannot get data on an input DGPort.", errorOut);
    return "";
  }
  FabricCore::Variant result = getVariant(slice, errorOut);
  if(result.isNull())
    return "";
  return result.getJSONEncoding().getStringData();
}

bool DGPortImpl::setJSON(const std::string & json, uint32_t slice, std::string * errorOut)
{
  if(mMode == Mode_OUT)
    return LoggingImpl::reportError("Cannot set data on an output DGPort.", errorOut);
  try
  {
    FabricCore::Variant variant = FabricCore::Variant::CreateFromJSON(json);
    return setVariant(variant, slice, errorOut);
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }
  DGGraphImplPtr node = getDGGraph();
  if(!node)
    return LoggingImpl::reportError("DGPortImpl::setJSON, Node '"+mGraphName+"' already destroyed.");
  node->requireEvaluate();
  return false;
}

FabricCore::Variant DGPortImpl::getDefault(std::string * errorOut)
{
  FabricCore::Variant result;
  if(mIsObject)
    return result;

  DGGraphImplPtr node = getDGGraph();
  if(!node)
  {
    LoggingImpl::reportError("DGPortImpl::getDefault, Node '"+mGraphName+"' already destroyed.");
    return result;
  }

  try
  {
    result = mDGNode.getMemberDefaultData_Variant(mMember.c_str());
    if(result.isNull())
      result = mDGNode.getMemberSliceData_Variant(mMember.c_str(), 0);
  }
  catch(FabricCore::Exception e)
  {
    LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  return result;
}

FabricCore::RTVal DGPortImpl::getRTVal(
  bool evaluate,
  uint32_t slice,
  std::string * errorOut
  )
{
  if(slice > getSliceCount())
  {
    LoggingImpl::reportError("Slice out of bounds.", errorOut);
    return FabricCore::RTVal();
  }
  DGGraphImplPtr node = getDGGraph();
  if(!node)
  {
    LoggingImpl::reportError("DGPortImpl::getRTVal, Node '"+mGraphName+"' already destroyed.");
    return FabricCore::RTVal();
  }

  if(mMode != Mode_IN && evaluate)
    if(!node->evaluate(mDGNode, errorOut))
      return FabricCore::RTVal();

  try
  {
    FabricCore::RTVal result =
      mDGNode.getMemberSliceValue(mMember.c_str(), slice);
    return result;
  }
  catch(FabricCore::Exception e)
  {
    LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }
  return FabricCore::RTVal();
}

bool DGPortImpl::setRTVal(
  FabricCore::RTVal value,
  uint32_t slice,
  std::string * errorOut
  )
{
  // if(mMode == Mode_OUT)
  //   return LoggingImpl::reportError("Cannot set data on an output DGPort.", errorOut);
  if(slice > mDGNode.getSize())
    return LoggingImpl::reportError("Slice out of bounds.", errorOut);

  try
  {
    mDGNode.setMemberSliceValue(mMember.c_str(), slice, value);
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }
  DGGraphImplPtr node = getDGGraph();
  if(!node)
    return LoggingImpl::reportError("DGPortImpl::setRTVal, Node '"+mGraphName+"' already destroyed.");
  node->requireEvaluate();
  return true;
}

uint32_t DGPortImpl::getArrayCount(uint32_t slice, std::string * errorOut)
{
  if(mMode == Mode_IN)
  {
    LoggingImpl::reportError("Cannot get data on an input DGPort.", errorOut);
    return 0;
  }
  if(!mIsArray)
    return 1;

  if(slice > getSliceCount())
  {
    LoggingImpl::reportError("Slice out of bounds.", errorOut);
    return 0;
  }
  DGGraphImplPtr node = getDGGraph();
  if(!node)
  {
    LoggingImpl::reportError("DGPortImpl::getArrayCount, Node '"+mGraphName+"' already destroyed.");
    return 0;
  }
  if(!node->evaluate(mDGNode, errorOut))
    return 0;

  try
  {
    uint32_t result = mDGNode.getMemberSliceArraySize(mMember.c_str(), slice);
    return result;
  }
  catch(FabricCore::Exception e)
  {
    LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }
  return 0;
}

bool DGPortImpl::getArrayData(
  void * buffer,
  uint32_t bufferSize,
  uint32_t slice,
  std::string * errorOut
  )
{
  if(mMode == Mode_IN)
    return LoggingImpl::reportError("Cannot get data on an input DGPort.", errorOut);
  if(!mIsArray)
    return LoggingImpl::reportError("DGPort is not an array.", errorOut);
  if(!mIsShallow)
    return LoggingImpl::reportError("DGPort is not shallow.", errorOut);
  if(slice > getSliceCount())
    return LoggingImpl::reportError("Slice out of bounds.", errorOut);
  if(buffer == NULL && bufferSize != 0)
    return LoggingImpl::reportError("No valid buffer / bufferSize provided.", errorOut);
  DGGraphImplPtr node = getDGGraph();
  if(!node)
    return LoggingImpl::reportError("DGPortImpl::getArrayData, Node '"+mGraphName+"' already destroyed.");
  if(!node->evaluate(mDGNode, errorOut))
    return false;

  uint32_t count = 0;
  try
  {
    count = mDGNode.getMemberSliceArraySize(mMember.c_str(), slice);
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  uint32_t bufferCount = bufferSize / mDataSize;
  if(bufferCount * mDataSize != bufferSize)
    return LoggingImpl::reportError("Invalid buffer size.", errorOut);
  if(bufferCount != count)
    return LoggingImpl::reportError("The buffer size does not match the array size.", errorOut);

  try
  {
    mDGNode.getMemberSliceArrayData(mMember.c_str(), slice, bufferSize, buffer);
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  return true;
}

bool DGPortImpl::setArrayData(
  void * buffer,
  uint32_t bufferSize,
  uint32_t slice,
  std::string * errorOut
  )
{
  if(mMode == Mode_OUT)
    return LoggingImpl::reportError("Cannot set data on an output DGPort.", errorOut);
  if(!mIsArray)
    return LoggingImpl::reportError("DGPort is not an array.", errorOut);
  if(!mIsShallow)
    return LoggingImpl::reportError("DGPort is not shallow.", errorOut);
  if(slice > getSliceCount())
    return LoggingImpl::reportError("Slice out of bounds.", errorOut);
  if(buffer == NULL && bufferSize != 0)
    return LoggingImpl::reportError("No valid buffer / bufferSize provided.", errorOut);

  uint32_t bufferCount = bufferSize / mDataSize;
  if(bufferCount * mDataSize != bufferSize)
    return LoggingImpl::reportError("Invalid buffer size.", errorOut);

  try
  {
    if ( mDGNode.getMemberSliceArraySize( mMember.c_str(), slice ) != bufferCount )
      mDGNode.setMemberSliceArraySize( mMember.c_str(), slice, bufferCount );
    if(bufferCount >  0)
      mDGNode.setMemberSliceArrayData(mMember.c_str(), slice, bufferSize, buffer);
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  DGGraphImplPtr node = getDGGraph();
  if(!node)
    return LoggingImpl::reportError("DGPortImpl::setArrayData, Node '"+mGraphName+"' already destroyed.");
  node->requireEvaluate();
  return true;
}

bool DGPortImpl::getAllSlicesData(void * buffer, uint32_t bufferSize, std::string * errorOut)
{
  if(mMode == Mode_IN)
    return LoggingImpl::reportError("Cannot get data on an input DGPort.", errorOut);
  if(mIsArray)
    return LoggingImpl::reportError("DGPort is an array.", errorOut);
  if(!mIsShallow)
    return LoggingImpl::reportError("DGPort is not shallow.", errorOut);
  if(buffer == NULL && bufferSize != 0)
    return LoggingImpl::reportError("No valid buffer / bufferSize provided.", errorOut);
  DGGraphImplPtr node = getDGGraph();
  if(!node)
    return LoggingImpl::reportError("DGPortImpl::getAllSlicesData, Node '"+mGraphName+"' already destroyed.");
  if(!node->evaluate(mDGNode, errorOut))
    return false;

  uint32_t sliceCount = mDGNode.getSize();
  if(sliceCount * mDataSize != bufferSize)
    return LoggingImpl::reportError("Buffer size does not match slice count.", errorOut);

  try
  {
    mDGNode.getMemberAllSlicesData(mMember.c_str(), bufferSize, buffer);
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  return true;
}

bool DGPortImpl::setAllSlicesData(void * buffer, uint32_t bufferSize, std::string * errorOut)
{
  if(mMode == Mode_OUT)
    return LoggingImpl::reportError("Cannot set data on an output DGPort.", errorOut);
  if(mIsArray)
    return LoggingImpl::reportError("DGPort is an array.", errorOut);
  if(!mIsShallow)
    return LoggingImpl::reportError("DGPort is not shallow.", errorOut);
  if(buffer == NULL && bufferSize != 0)
    return LoggingImpl::reportError("No valid buffer / bufferSize provided.", errorOut);

  uint32_t sliceCount = mDGNode.getSize();
  uint32_t bufferCount = bufferSize / mDataSize;
  if(bufferCount * mDataSize != bufferSize)
    return LoggingImpl::reportError("Invalid buffer size.", errorOut);
  if(bufferCount != sliceCount)
    return LoggingImpl::reportError("The buffer size does not match the slice count.", errorOut);

  try
  {
    mDGNode.setMemberAllSlicesData(mMember.c_str(), bufferSize, buffer);
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  DGGraphImplPtr node = getDGGraph();
  if(!node)
    return LoggingImpl::reportError("DGPortImpl::setAllSlicesData, Node '"+mGraphName+"' already destroyed.");
  node->requireEvaluate();
  return true;
}

bool DGPortImpl::copyArrayDataFromDGPort(DGPortImplPtr other, uint32_t slice, uint32_t otherSliceHint, std::string * errorOut)
{
  if(mMode == Mode_OUT)
    return LoggingImpl::reportError("Cannot set data on an output DGPort.", errorOut);
  if(!mIsArray)
    return LoggingImpl::reportError("DGPort is not an array.", errorOut);
  if(!mIsShallow)
    return LoggingImpl::reportError("DGPort is not shallow.", errorOut);
  if(!other)
    return LoggingImpl::reportError("Other DGPort is not valid.", errorOut);
  if(other->mMode == Mode_IN)
    return LoggingImpl::reportError("Other DGPort is not an out DGPort.", errorOut);
  if(!other->mIsShallow)
    return LoggingImpl::reportError("Other DGPort is not shallow.", errorOut);
  if(!other->mIsArray)
    return LoggingImpl::reportError("Other DGPort is not an array.", errorOut);
  if(other->mDataType != mDataType)
    return LoggingImpl::reportError("DGPorts' data types don't match.", errorOut);
  if(other->mDataSize != mDataSize)
    return LoggingImpl::reportError("DGPorts' data sizes don't match.", errorOut);
  if(slice > getSliceCount())
    return LoggingImpl::reportError("Slice out of bounds.", errorOut);

  DGGraphImplPtr otherNode = other->getDGGraph();
  if(!otherNode)
    return LoggingImpl::reportError("DGPortImpl::copyArrayDataFromDGPort, Node '"+other->mGraphName+"' already destroyed.");
  if(!otherNode->evaluate(other->mDGNode, errorOut))
    return false;

  uint32_t otherSlice = otherSliceHint;
  if(otherSlice == UINT_MAX)
    otherSlice = slice;
  if(otherSlice > other->getSliceCount())
    return LoggingImpl::reportError("Other DGPort's slice out of bounds.", errorOut);

  uint32_t arrayCount = 0;
  try
  {
    arrayCount = other->mDGNode.getMemberSliceArraySize(other->mMember.c_str(), otherSlice);
  }
  catch(FabricCore::Exception e)
  {
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  uint32_t bufferSize = mDataSize * arrayCount;
  void * buffer = NULL;

  if(bufferSize > 0)
  {
    buffer = malloc(bufferSize);
    try
    {
      other->mDGNode.getMemberSliceArrayData(
        other->mMember.c_str(), otherSlice, bufferSize, buffer
        );
    }
    catch(FabricCore::Exception e)
    {
      free(buffer);
      return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
    }
  }

  try
  {
    mDGNode.setMemberSliceArraySize(mMember.c_str(), slice, arrayCount);
    if(bufferSize >  0)
      mDGNode.setMemberSliceArrayData(mMember.c_str(), slice, bufferSize, buffer);
  }
  catch(FabricCore::Exception e)
  {
    free(buffer);
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  free(buffer);
  DGGraphImplPtr node = getDGGraph();
  if(!node)
    return LoggingImpl::reportError("DGPortImpl::copyArrayDataFromDGPort, Node '"+mGraphName+"' already destroyed.");
  node->requireEvaluate();
  return true;
}

bool DGPortImpl::copyAllSlicesDataFromDGPort(DGPortImplPtr other, bool resizeTarget, std::string * errorOut)
{
  if(mMode == Mode_OUT)
    return LoggingImpl::reportError("Cannot set data on an output DGPort.", errorOut);
  if(mIsArray)
    return LoggingImpl::reportError("DGPort is an array.", errorOut);
  if(!mIsShallow)
    return LoggingImpl::reportError("DGPort is not shallow.", errorOut);
  if(!other)
    return LoggingImpl::reportError("Other DGPort is not valid.", errorOut);
  if(other->mMode == Mode_IN)
    return LoggingImpl::reportError("Other DGPort is not an out DGPort.", errorOut);
  if(!other->mIsShallow)
    return LoggingImpl::reportError("Other DGPort is not shallow.", errorOut);
  if(other->mIsArray)
    return LoggingImpl::reportError("Other DGPort is an array.", errorOut);
  if(other->mDataType != mDataType)
    return LoggingImpl::reportError("DGPorts' data types don't match.", errorOut);
  if(other->mDataSize != mDataSize)
    return LoggingImpl::reportError("DGPorts' data sizes don't match.", errorOut);

  uint32_t sliceCount = other->mDGNode.getSize();
  if(sliceCount != mDGNode.getSize())
  {
    if(resizeTarget)
      mDGNode.setSize(sliceCount);
    else
      return LoggingImpl::reportError("Slice counts don't match.", errorOut);
  }

  if(sliceCount == 0)
    return true;

  DGGraphImplPtr otherNode = other->getDGGraph();
  if(!otherNode)
    return LoggingImpl::reportError("DGPortImpl::copyAllSlicesDataFromDGPort, Node '"+other->mGraphName+"' already destroyed.");
  if(!otherNode->evaluate(other->mDGNode, errorOut))
    return false;

  uint32_t bufferSize = mDataSize * sliceCount;
  void * buffer = malloc(bufferSize);

  try
  {
    other->mDGNode.getMemberAllSlicesData(other->mMember.c_str(), bufferSize, buffer);
  }
  catch(FabricCore::Exception e)
  {
    free(buffer);
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  try
  {
    mDGNode.setMemberAllSlicesData(mMember.c_str(), bufferSize, buffer);
  }
  catch(FabricCore::Exception e)
  {
    free(buffer);
    return LoggingImpl::reportError(e.getDesc_cstr(), errorOut);
  }

  free(buffer);
  DGGraphImplPtr node = getDGGraph();
  if(!node)
    return LoggingImpl::reportError("DGPortImpl::copyAllSlicesDataFromDGPort, Node '"+mGraphName+"' already destroyed.");
  node->requireEvaluate();
  return true;
}

void DGPortImpl::setOption(const std::string & name, const FabricCore::Variant & value)
{
  std::map<std::string,FabricCore::Variant>::iterator it = mOptions.find(name);
  if(it == mOptions.end())
    mOptions.insert(std::pair<std::string,FabricCore::Variant>(name, value));
  else
    it->second = value;
}

FabricCore::Variant DGPortImpl::getOption(const std::string & name)
{
  std::map<std::string,FabricCore::Variant>::iterator it = mOptions.find(name);
  if(it == mOptions.end())
    return FabricCore::Variant();
  return it->second;
}

FabricCore::Variant DGPortImpl::getAllOptions()
{
  FabricCore::Variant result;
  if(mOptions.size() == 0)
    return result;
  result = FabricCore::Variant::CreateDict();
  std::map<std::string,FabricCore::Variant>::iterator it = mOptions.begin();
  for(;it!=mOptions.end();it++)
  {
    result.setDictValue(it->first.c_str(), it->second);
  }

  return result;
}
