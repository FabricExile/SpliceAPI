// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#include <FabricSplice.h>
#include <vector>

using namespace FabricSplice;

#define FABRIC_TRY(code) try { code } catch(FabricSplice::Exception e) { printf("Exception: %s\n", e.what()); }

int main( int argc, const char* argv[] )
{
  Initialize();

  // create a graph
  DGGraph graph = DGGraph("myGraph");

  // create a DGNode
  graph.constructDGNode();;

  // create a member
  graph.addDGNodeMember("value", "Scalar");

  // create a port
  // the mode defines if a port can be read from, 
  // written to or both (like in this case)
  graph.addDGPort("value", "value", Port_Mode_IO);

  // create an operator to do math compute
  std::string klCode = "operator reportScalarOp(Scalar value) {\n";
  klCode += "  report(value);\n";
  klCode += "}\n";
  graph.constructKLOperator("reportScalarOp", klCode.c_str()); // default onto the DGNode 

  // pull
  graph.evaluate();

  // remove the op
  FABRIC_TRY( graph.removeKLOperator("reportScalarOp"); )

  // re-add the operator
  FABRIC_TRY( graph.constructKLOperator("reportScalarOp", klCode.c_str()); )

  // pull
  FABRIC_TRY( graph.evaluate(); )

  // create fault kl code
  std::string faultyKlCode = klCode + "some faulty lines;\n";
  FABRIC_TRY( graph.setKLOperatorSourceCode("reportScalarOp", faultyKlCode.c_str()); )

  // remove it once more
  FABRIC_TRY( graph.removeKLOperator("reportScalarOp"); )

  // re-add the operator
  FABRIC_TRY( graph.constructKLOperator("reportScalarOpNewName", klCode.c_str(), "reportScalarOp"); )

  Finalize();
  return 0;
}

