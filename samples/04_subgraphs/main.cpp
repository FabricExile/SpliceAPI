// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#include <FabricSplice.h>

using namespace FabricSplice;

int main( int argc, const char* argv[] )
{
  std::string klCode;

  Initialize();

  // create a graph
  DGGraph graph = DGGraph("myGraph");  

  // create the core DG nodes
  graph.constructDGNode("DGNodeA");
  graph.constructDGNode("DGNodeB");
  graph.constructDGNode("DGNodeC");

  // setup dependencies
  graph.setDGNodeDependency("DGNodeA", "DGNodeB");
  graph.setDGNodeDependency("DGNodeA", "DGNodeC");

  // create some members
  graph.addDGNodeMember("result", "Scalar"); // uses default DGNode
  graph.addDGNodeMember("valueA", "Scalar", FabricCore::Variant(), "DGNodeB");
  graph.addDGNodeMember("valueB", "Scalar", FabricCore::Variant(), "DGNodeC");

  // create ports to the members
  DGPort sum = graph.addDGPort("sum", "result", Port_Mode_IO); // uses default DGNode
  DGPort A = graph.addDGPort("A", "valueA", Port_Mode_IO, "DGNodeB");
  DGPort B = graph.addDGPort("B", "valueB", Port_Mode_IO, "DGNodeC");

  // create an operator to do math compute
  klCode = "operator addTwoScalarsOp(Scalar A, Scalar B, io Scalar sum) {\n";
  klCode += "  sum = A + B;\n";
  klCode += "}\n";
  graph.constructKLOperator("addTwoScalarsOp", klCode.c_str()); // default onto the DGNode

  // create an operator to do value logging, 
  klCode = "operator reportScalar(Scalar value) {\n";
  klCode += "  report(value);\n";
  klCode += "}\n";

  // make use of the port remapping, so that the same operator
  // logs different port members. this will also use branch
  // based multithreading.
  FabricCore::Variant portMap = FabricCore::Variant::CreateDict();
  portMap.setDictValue("value", FabricCore::Variant::CreateString("sum"));
  graph.constructKLOperator("reportScalar", klCode.c_str(), "reportScalar", "DGNodeA", portMap);
  portMap.setDictValue("value", FabricCore::Variant::CreateString("A"));
  graph.constructKLOperator("reportScalar", klCode.c_str(), "reportScalar", "DGNodeB", portMap);
  portMap.setDictValue("value", FabricCore::Variant::CreateString("B"));
  graph.constructKLOperator("reportScalar", klCode.c_str(), "reportScalar", "DGNodeC", portMap);

  // feed in some data and perform
  A.setVariant(FabricCore::Variant::CreateFloat32(1.0));
  B.setVariant(FabricCore::Variant::CreateFloat32(2.0));
  FabricCore::Variant resultVar = sum.getVariant();
  printf("Result: %s\n", resultVar.getJSONEncoding().getStringData());

  graph.removeDGNodeDependency("DGNodeA", "DGNodeC");

  Finalize();
  return 0;
}

