#include "graphColor.h"
#include "State.h"
#include "graph.h"


/*readonly*/ 
CProxy_Main mainProxy;
//TO DO: To Make  graph and i2vMap globally shared
//Graph graph;
//VextexMap i2vMap;


Main::Main(CkArgMsg* msg) {

  if(msg->argc < 1) {
    CkPrintf("\n\n\nUsage: <exec> filename\n");
    CkExit();
  }

  Graph graph;
  VextexMap i2vMap;

  /* Parse the file and populate the graph*/
  parseCommandLine(msg->argv[1],graph,i2vMap);

  std::cout << graph;  
  mainProxy= thisProxy;

  /* TO DO: Select a vertex and create K state nodes, where
   * k is the possible colors and fire a chare on each one 
   * of them
   */




  delete msg;
  CkExit(); // TO BE DELETED
}

Main::Main(CkMigrateMessage* msg) {}

void Main::done() { 
  CkExit();
}

#include "Module.def.h" 
