#include "graphColor.h"

/*readonly*/ 
CProxy_Main mainProxy;
CProxy_Node nodeProxy;

Main::Main(CkArgMsg* msg) {

  if(msg->argc < 1) {
    CkPrintf("\n\n\nUsage: <exec> filename\n");
    CkExit();
  }

  /* Parse the file and populate the graph*/
  parseCommandLine(msg->argv[1]);


  mainProxy= thisProxy;
  delete msg;
}

Main::Main(CkMigrateMessage* msg) {}

void Main::done() { 
  CkExit();
}

#include "Module.def.h" 
