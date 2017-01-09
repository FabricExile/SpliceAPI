// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#include <FabricSplice.h>

using namespace FabricSplice;

int main( int argc, const char* argv[] )
{
  Initialize();

  // create a graph to store the nodes
  DGGraph graph = DGGraph("myGraph");

  // create two DG nodes
  // the first DG node is established
  // as the 'default' DGNode  graph.constructDGNode("compute"); // uses default name
  graph.constructDGNode("compute");
  graph.constructDGNode("data");

  // create a dependency
  graph.setDGNodeDependency("compute", "data");

  // create the members to connect
  graph.addDGNodeMember("someValues", "Scalar[]", FabricCore::Variant(), "data");
  graph.addDGNodeMember("sum", "Scalar"); // default DGNode
  graph.addDGNodeMember("product", "Scalar"); // default DGNode

  // create ports
  DGPort someValues = graph.addDGPort("someValues", "someValues", Port_Mode_IN, "data");
  DGPort sum = graph.addDGPort("sum", "sum", Port_Mode_OUT); // default DGNode
  DGPort product = graph.addDGPort("product", "product", Port_Mode_OUT); // default DGNode

  // create an operator
  std::string klCode = "";
  klCode += "operator addOp(Scalar someValues[], io Scalar sum) {";
  klCode += "  sum = 0.0;";
  klCode += "  for(Size i=0;i<someValues.size();i++)";
  klCode += "    sum += someValues[i];";
  klCode += "}";
  graph.constructKLOperator("addOp", klCode.c_str());

  // create another operator
  klCode = "";
  klCode += "operator mulOp(Scalar someValues[], io Scalar product) {";
  klCode += "  product = 1.0;";
  klCode += "  for(Size i=0;i<someValues.size();i++)";
  klCode += "    product *= someValues[i];";
  klCode += "}";
  graph.constructKLOperator("mulOp", klCode.c_str());

  // persist the data to json
  // (alternatively, you can also call the saveToFile method to persist to a file)
  std::string jsonData = graph.getPersistenceDataJSON();

  // destroy the ports, the node and close the FabricCore::Client
  graph = DGGraph();

  // print the persisted data
  printf("%s\n", jsonData.c_str());

  // create a new graph
  graph = DGGraph("anotherGraph");

  // reconstruct the members, ports and operators!
  // (alternatively, you can also call the loadFromFile method to persist from a file)
  graph.setFromPersistenceDataJSON(jsonData);

  // print all of the port names
  for(unsigned int i=0;i<graph.getDGPortCount();i++)
    printf("port reconstructed: %s\n", graph.getDGPortName(i));

  // print all of the operator names
  for(unsigned int i=0;i<graph.getKLOperatorCount();i++)
    printf("operator reconstructed: %s\n", graph.getKLOperatorName(i));

  Finalize();
  return 0;
}

