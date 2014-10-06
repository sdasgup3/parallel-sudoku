#include "graphColor.h"

/*readonly*/ 
CProxy_Main mainProxy;
CProxy_Node nodeProxy;

Main::Main(CkArgMsg* msg) {

  if(msg->argc < 1) {
    CkPrintf("\n\n\nUsage: <exec> chare_array_size min max\n");
    CkPrintf("\twhere chare_array_size > 0 \n\n\n");
    CkExit();
  }

  CkPrintf("%d", atoi(msg->argv[1]));

  mainProxy= thisProxy;
  delete msg;
}

Main::Main(CkMigrateMessage* msg) {}

void Main::done() { 
  CkExit();
}

#include "Module.def.h" 
