// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#include <FabricSplice.h>
#include <vector>

using namespace FabricSplice;

int main( int argc, const char* argv[] )
{
  Initialize();

  // create a graph
  DGGraph graph = DGGraph("myGraph");

  // create a DGNode
  graph.constructDGNode();;

  // create a member
  graph.addDGNodeMember("myScalar", "Scalar");

  // create a port
  // the mode defines if a port can be read from, 
  // written to or both (like in this case)
  DGPort port = graph.addDGPort("myPortName", "myScalar", Port_Mode_IO);

  // set some data on the port using JSON
  port.setJSON("1.3");

  // print the data
  printf("%s\n", port.getJSON().c_str());

  // set some data on the port using a variant
  port.setVariant(FabricCore::Variant::CreateFloat64(2.6));

  // print the data
  printf("%s\n", port.getJSON().c_str());

  // nodes can store any number of values. the count of the values
  // is referred to as 'slice count'
  port.setSliceCount(3);
  port.setVariant(FabricCore::Variant::CreateFloat64(1.6), 0);
  port.setVariant(FabricCore::Variant::CreateFloat64(2.6), 1);
  port.setVariant(FabricCore::Variant::CreateFloat64(3.6), 2);

  // print the data
  for(uint32_t i=0;i<port.getSliceCount();i++)
    printf("%x: %s\n", i, port.getJSON(i).c_str());

  // aside from json and variants you can also use the 
  // high performance IO on a port.
  std::vector<float> values(5);
  values[0] = 3.4;
  values[1] = 4.5;
  values[2] = 5.6;
  values[3] = 6.7;
  values[4] = 7.8;
  port.setSliceCount(values.size());
  port.setAllSlicesData(&values[0], sizeof(float) * values.size());

  // clear the values
  values.clear();
  values.resize(port.getSliceCount());

  // get the data again using the high performance IO
  port.getAllSlicesData(&values[0], sizeof(float) * values.size());

  // print the retured data
  for(uint32_t i=0;i<values.size();i++) 
    printf("%x: %f\n", i, values[i]);

  // create an operator to do math compute
  std::string klCode = "operator reportScalarOp(Scalar myPortName) {\n";
  klCode += "  report(myPortName);\n";
  klCode += "}\n";
  graph.constructKLOperator("reportScalarOp", klCode.c_str()); // default onto the DGNode 

  // remove the port
  // this will make the operator invalid
  graph.removeDGNodeMember("myScalar");
  graph.removeDGPort("myPortName");

  // evaluate, this will throw errors since the binding isn't right
  try
  {
    graph.evaluate();
  }
  catch(FabricSplice::Exception e)
  {
    printf("%s\n", e.what());
  }

  // readd the member and the port
  graph.addDGNodeMember("myScalar", "Scalar");
  port = graph.addDGPort("myPortName", "myScalar", Port_Mode_IO);

  graph.evaluate();

  // get the data again using the high performance IO (should all be zeroes)
  port.getAllSlicesData(&values[0], sizeof(float) * values.size());

  // print the retured data
  for(uint32_t i=0;i<values.size();i++) 
    printf("%x: %f\n", i, values[i]);

  Finalize();
  return 0;
}

