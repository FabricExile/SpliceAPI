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

  // report versions
  printf("Core Version: %s\n", FabricSplice::GetFabricVersionStr());
  printf("Splice Version: %s\n", FabricSplice::GetSpliceVersion());

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

