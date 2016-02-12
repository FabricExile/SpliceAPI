// Copyright 2010-2013 Fabric Engine Inc. All rights reserved.

#ifndef __FabricSpliceImpl_TYPEDEFS_H__
#define __FabricSpliceImpl_TYPEDEFS_H__

#include <vector>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

namespace FabricSpliceImpl
{
  /// Forward declarations
  class DGGraphImpl;
  typedef boost::shared_ptr<DGGraphImpl> DGGraphImplPtr;
  typedef boost::weak_ptr<DGGraphImpl> DGGraphImplWeakPtr;
  typedef std::vector<DGGraphImplPtr> DGGraphImplPtrVector;
  class DGPortImpl;
  typedef boost::shared_ptr<DGPortImpl> DGPortImplPtr;
  typedef boost::weak_ptr<DGPortImpl> DGPortImplWeakPtr;
  typedef std::vector<DGPortImplPtr> DGPortImplPtrVector;
};

#endif
