// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#include <FabricSplice.h>

using namespace FabricSplice;

int main( int argc, const char* argv[] )
{
  Initialize();

  // enable timers
  Logging::enableTimers();
  Logging::AutoTimer overallTime("overall");

  // create a graph to store the nodes
  DGGraph graph = DGGraph("myGraph");
  graph.constructDGNode();

  // create the members to connect
  graph.addDGNodeMember("scalar", "Scalar");
  graph.addDGNodeMember("integer", "Integer");
  graph.addDGNodeMember("vec3", "Vec3");
  graph.addDGNodeMember("mat44", "Mat44");
  graph.addDGNodeMember("xfo", "Xfo");

  // create ports
  DGPort scalar = graph.addDGPort("scalar", "scalar", Port_Mode_IO);
  DGPort integer = graph.addDGPort("integer", "integer", Port_Mode_IO);
  DGPort vec3 = graph.addDGPort("vec3", "vec3", Port_Mode_IO);
  DGPort mat44 = graph.addDGPort("mat44", "mat44", Port_Mode_IO);
  DGPort xfo = graph.addDGPort("xfo", "xfo", Port_Mode_IO);

  // create an operator to do value logging, 
  std::string klCode;
  klCode = "operator reportValues(Scalar scalar, Integer integer, Vec3 vec3, Mat44 mat44, Xfo xfo) {\n";
  klCode += "  report(scalar);\n";
  klCode += "  report(integer);\n";
  klCode += "  report(vec3);\n";
  klCode += "  report(mat44);\n";
  klCode += "  report(xfo);\n";
  klCode += "}\n";
  graph.constructKLOperator("reportValues", klCode.c_str());

  // evaluate the graph
  graph.evaluate();

  // report all timers
  for(unsigned int i=0;i<Logging::getNbTimers();i++)
  {
    Logging::logTimer(Logging::getTimerName(i));
  }

  Finalize();
  return 0;
}

