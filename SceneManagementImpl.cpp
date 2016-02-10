// Copyright 2010-2013 Fabric Engine Inc. All rights reserved.

#include "SceneManagementImpl.h"
#include "LoggingImpl.h"
#include "DGGraphImpl.h"

#include <stdio.h>
#include <float.h>

using namespace FabricSpliceImpl;

bool SceneManagementImpl::sInError = false;

bool SceneManagementImpl::hasRenderableContent()
{
  return DGGraphImpl::sAllDGGraphs.size() > 0;
}

void SceneManagementImpl::drawOpenGL(FabricCore::RTVal & drawContext)
{
  if(sInError)
    return;
  try
  {
    DGGraphImpl::sDrawingScope.callMethod("", "draw", 1, &drawContext);
  }
  catch(FabricCore::Exception e)
  {
  }
}

void SceneManagementImpl::draw(FabricCore::RTVal & drawContext, std::vector<FabricCore::RTVal> &params)
{
  if(sInError)
    return;
  try
  {
    //DGGraphImpl::sDrawingScope.callMethod("", "draw", 1, &drawContext);
    DGGraphImpl::sMayaToRTRCallback.callMethod("", "render", 4, &params[0]);   
  }
  catch(FabricCore::Exception e)
  {
  }
}


void SceneManagementImpl::setErrorStatus(bool inError)
{
  sInError = inError;
}
