// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __FabricSpliceImpl_PORTIMPL_H__
#define __FabricSpliceImpl_PORTIMPL_H__

#include "LoggingImpl.h"
#include "ObjectImpl.h"
#include "TypeDefs.h"
#include <FabricCore.h>

#include <limits.h>

namespace FabricSpliceImpl
{
  class DGPortImpl : public ObjectImpl
  {
    friend class DGGraphImpl;
    
  public:

    enum Lock
    {
        Lock_Perform,
        Lock_Omit
    };

    enum Mode
    {
      Mode_IN,
      Mode_OUT,
      Mode_IO
    };

    /*
      Constructors / Destructors
    */

    virtual ~DGPortImpl();

    /*
      Basic data getters
    */

    /// returns the name of the member this DGPort is connected to
    char const * getMember() const { return mMember.c_str(); }

    /// returns the name of the FabricCore::DGNode this DGPort is connected to
    char const * getDGNodeName() const { return mDGNodeName.c_str(); }

    /// returns a unique key descripting the DGPort
    char const * getKey() const { return mKey.c_str(); }

    /// returns the FabricSpliceImpl::DGGraph this DGPort belongs to
    DGGraphImplPtr getDGGraph();

    /// returns the mode of this DGPort
    Mode getMode() const { return mMode; }

    /// sets the mode of this DGPort
    void setMode(Mode mode) { mMode = mode; }

    /// returns the data type of the member this DGPort is connected to
    char const * getDataType() const { return mDataType.c_str(); }

    /// returns the data size of a single element of the member this DGPort is connected to.
    /// So for example, both for a 'Vec3' and 'Vec3[]' this will return sizeof(Vec3) == 12
    uint32_t getDataSize() const { return mDataSize; }

    /// returns true if the data type of this DGPort is shallow.
    /// only shallow data types can be used with the high performance IO
    bool isShallow() const  { return mIsShallow; }

    /// returns true if the data type of this DGPort is an array (Vec3[] for example)
    bool isArray() const { return mIsArray; }

    /// returns true if the data type of this DGPort is a struct
    bool isStruct() const { return mIsStruct; }

    /// returns true if the data type of this DGPort is an object
    bool isObject() const { return mIsObject; }

    /// returns true if the data type of this DGPort is an interface
    bool isInterface() const { return mIsInterface; }

    /// returns true if this port auto initializes KL objects
    bool doesAutoInitObjects() const { return mAutoInitObjects; }

    /*
      FabricCore slicing management
    */

    /// returns the slice count of the FabricCore::DGNode this DGPort is connected to
    uint32_t getSliceCount(std::string * errorOut = NULL);

    /// sets the slice count of the FabricCore::DGNode this DGPort is connected to
    bool setSliceCount(uint32_t count, std::string * errorOut = NULL);

    /*
      FabricCore::Variant IO
    */

    /// returns the value of a specific slice of this DGPort as a FabricCore::Variant
    FabricCore::Variant getVariant(uint32_t slice = 0, std::string * errorOut = NULL);

    /// sets the value of a specific slice of this DGPort from a FabricCore::Variant
    bool setVariant(FabricCore::Variant value, uint32_t slice = 0, std::string * errorOut = NULL);

    /// returns the value of a specific slice of this DGPort as a JSON string
    std::string getJSON(uint32_t slice = 0, std::string * errorOut = NULL);

    /// sets the value of a specific slice of this DGPort from a JSON string
    bool setJSON(const std::string & json, uint32_t slice = 0, std::string * errorOut = NULL);

    /// returns the default value of this DGPort as a FabricCore::Variant
    FabricCore::Variant getDefault(std::string * errorOut = NULL);

    /* 
      FabricCore::RTVal IO
    */

    /// returns the value of a specific slice of this DGPort as a FabricCore::RTVal
    FabricCore::RTVal getRTVal(
        bool evaluate = false,
        uint32_t slice = 0,
        std::string * errorOut = NULL
        );

    /// sets the value of a specific slice of this DGPort from a FabricCore::RTVal
    bool setRTVal(
        FabricCore::RTVal value,
        uint32_t slice = 0,
        std::string * errorOut = NULL
        );

    /*
      High Performance IO
      void* access to the internal data of the FabricCore::DGNode is only possible
      for shallow data types (like Vec3 for example).
    */

    /// returns the size of an array member this DGPort is connected to
    uint32_t getArrayCount(uint32_t slice = 0, std::string * errorOut = NULL);

    /// returns the void* array data of this DGPort.
    /// this only works for array DGPorts (isArray() == true)
    /// the bufferSize has to match getArrayCount() * getDataSize()
    bool getArrayData(
        void * buffer,
        uint32_t bufferSize,
        uint32_t slice = 0,
        std::string * errorOut = NULL
        );

    /// sets the void* array data of this DGPort.
    /// this only works for array DGPorts (isArray() == true)
    /// this also sets the array count determined by bufferSize / getDataSize()
    bool setArrayData(
        void * buffer,
        uint32_t bufferSize,
        uint32_t slice = 0,
        std::string * errorOut = NULL
        );

    /// gets the void* slice array data of this DGPort.
    /// this only works for non-array DGPorts (isArray() == false)
    /// the bufferSize has to match getSliceCount() * getDataSize()
    bool getAllSlicesData(void * buffer, uint32_t bufferSize, std::string * errorOut = NULL);

    /// sets the void* slice array data of this DGPort.
    /// this only works for non-array DGPorts (isArray() == false)
    /// the bufferSize has to match getSliceCount() * getDataSize()
    bool setAllSlicesData(void * buffer, uint32_t bufferSize, std::string * errorOut = NULL);

    /// set the array data based on another port
    /// this performs data replication, and only works on shallow array data ports.
    /// the data type has to match as well (so only Vec3 to Vec3 for example).
    bool copyArrayDataFromDGPort(DGPortImplPtr other, uint32_t slice = 0, uint32_t otherSlice = UINT_MAX, std::string * errorOut = NULL);

    /// set the slices data based on another port
    /// this performs data replication, and only works on shallow non array data ports.
    /// the data type has to match as well (so only Vec3 to Vec3 for example).
    bool copyAllSlicesDataFromDGPort(DGPortImplPtr other, bool resizeTarget = false, std::string * errorOut = NULL);

    /*
      Auxiliary option management
    */

    // sets an auxiliary option
    void setOption(const std::string & name, const FabricCore::Variant & value);

    // gets an auxiliary option (empty variant if not defined)
    FabricCore::Variant getOption(const std::string & name);

    // gets a dictionary of all options
    FabricCore::Variant getAllOptions();

  private:
    DGPortImpl(DGGraphImplPtr thisGraph, const std::string & name, const std::string & member, FabricCore::DGNode dgNode, const std::string & dgNodeName, Mode mode, uint32_t dataSize, bool shallow, bool autoInitObjects);

    DGGraphImplWeakPtr mGraph;
    std::string mGraphName;
    std::string mKey;
    std::string mMember;
    bool mAutoInitObjects;
    FabricCore::DGNode mDGNode;
    std::string mDGNodeName;
    Mode mMode;
    std::string mDataType;
    bool mIsShallow;
    bool mIsArray;
    bool mIsStruct;
    bool mIsObject;
    bool mIsInterface;
    uint32_t mDataSize;
    // int mManipulatable;
    std::map<std::string,FabricCore::Variant> mOptions;
  };

  typedef std::map<std::string, DGPortImplPtr> DGPortMap;
  typedef DGPortMap::iterator DGPortIt;
  typedef DGPortMap::const_iterator DGPortConstIt;
  typedef std::pair<std::string, DGPortImplPtr> DGPortPair;
};

#include "DGGraphImpl.h"

#endif
