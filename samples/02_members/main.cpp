// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#include <FabricSplice.h>

using namespace FabricSplice;

int main( int argc, const char* argv[] )
{
  Initialize();

  // create a graph
  DGGraph graph = DGGraph("myGraph");

  // create a DGNode
  graph.constructDGNode();

  // create some members
  graph.addDGNodeMember("myScalar", "Scalar");
  graph.addDGNodeMember("myScalarWithDefault", "Scalar", FabricCore::Variant::CreateFloat64(7.1));
  graph.addDGNodeMember("myString", "String");
  graph.addDGNodeMember("v", "Vec3");

  Finalize();
  return 0;
}

