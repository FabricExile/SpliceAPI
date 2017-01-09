// Copyright (c) 2010-2017 Fabric Software Inc. All rights reserved.
#include <FabricSplice.h>

using namespace FabricSplice;

void myLogFunc(const char * message, unsigned int length)
{
  printf("[MyCallback] %s\n", message);
}

void myLogErrorFunc(const char * message, unsigned int length)
{
  printf("[MyCallback] Error: %s\n", message);
}

void myCompilerErrorFunc(
  unsigned int row, 
  unsigned int col, 
  const char * file,
  const char * level,
  const char * desc
) {
  printf("[MyCallback] KL Error: %s, Line %d, Col %d: %s\n", file, (int)row, (int)col, desc);
}

void myKLReportFunc(const char * message, unsigned int length)
{
  printf("[MyCallback] KL Reports: %s\n", message);
}

void myKLStatusFunc(const char * topic, unsigned int topicLength,  const char * message, unsigned int messageLength)
{
  printf("[MyCallback] KL Status for '%s': %s\n", topic, message);
}

int main( int argc, const char* argv[] )
{
  Initialize();

  // setup the callback functions
  Logging::setLogFunc(myLogFunc);
  Logging::setLogErrorFunc(myLogErrorFunc);
  Logging::setCompilerErrorFunc(myCompilerErrorFunc);
  Logging::setKLReportFunc(myKLReportFunc);
  Logging::setKLStatusFunc(myKLStatusFunc);

  // create a graph
  DGGraph graph = DGGraph("myGraph");

  // create a DGNode
  graph.constructDGNode();

  // create a member
  graph.addDGNodeMember("value", "Vec3");

  // create a port
  DGPort port = graph.addDGPort("value", "value", Port_Mode_IO);

  // create an op
  graph.constructKLOperator("testOp");

  // on purpose create a compiler error
  try
  {
    graph.setKLOperatorSourceCode("testOp", "operator testOp(Vec3 value) {adsadsd;}");
  }
  catch(Exception e)
  {
    printf("Caught error: %s\n", e.what());
  }

  // update the operator to report from KL
  graph.setKLOperatorSourceCode("testOp", "operator testOp() { report('my message');}");

  // evaluate will invoke all operators
  // and in this case also call the myKLReportFunc
  graph.evaluate();

  // update the operator to send a status update from KL
  graph.setKLOperatorSourceCode("testOp", "operator testOp() { queueStatusMessage('info', 'nothing going on!');}");

  // evaluate will invoke all operators
  // and in this case also call the myKLStatusFunc
  graph.evaluate();

  Finalize();
  return 0;
}

