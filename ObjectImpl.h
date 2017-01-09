// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.

#ifndef __FabricSpliceImpl_OBJECT_H__
#define __FabricSpliceImpl_OBJECT_H__

#include <string>

namespace FabricSpliceImpl
{
  class ObjectImpl
  {
  public:

    ObjectImpl() {}
    virtual ~ObjectImpl() {}

    virtual std::string getName() const { return mName; }
    virtual char const * getName_cstr() const { return mName.c_str(); }
    virtual bool setName(const std::string & name) { mName = name; return true; }

  private:
    std::string mName;
  };
};

#endif
