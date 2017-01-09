// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __FabricSpliceImpl__SceneManagementImpl_H__
#define __FabricSpliceImpl__SceneManagementImpl_H__

#include "StringUtilityImpl.h"
#include "DGPortImpl.h"

namespace FabricSpliceImpl
{
  // struct ManipulationData {
  //   int event;
  //   void * userData;
  //   const char * graphName;
  //   const char * portName;
  //   const FabricCore::RTVal * manipulationContext;
  //   const FabricCore::RTVal * manipulationResult;
  // };

  /// is fired when a manipulation has happened
  // typedef int(*ManipulationFunc)(ManipulationData * data);

  class SceneManagementImpl
  {
  public:

    // /// sets the callback for manipulation
    // static void setManipulationFunc(ManipulationFunc func);

    // /// fires the manipulation callback
    // static void callManipulationFunc(ManipulationData * data);

    /// returns true if a type is manipulatable
    /// this essentially checks if a the type has a method with 
    /// the following notation: 
    /// function Integer CustomType.manipulate!(String prefix, ManipulationContext context, io ManipulationResult result)
    // static bool isKLTypeManipulatable(std::string name);

    /// returns true if there is anything to render
    static bool hasRenderableContent();

    /// draw all drawable ports
    static void drawOpenGL(FabricCore::RTVal & drawContext);

    /// raycast against all raycastable objects
    // static bool raycast(FabricCore::RTVal & raycastContext, DGPortImplPtr & port);

    /// resets all cached data
    // static void reset();

    /// set error status
    static void setErrorStatus(bool inError);

  private:
    // static ManipulationFunc sManipulationFunc;
    // static std::map<std::string, int> sManipulatableMap;
    static bool sInError;
  };
}

#endif
