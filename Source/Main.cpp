#include "graphColor.h"


CProxy_Main mainProxy;
AdjListType adjList_;
int vertices_;

Main::Main(CkArgMsg* msg) {

  if(msg->argc < 1) {
    CkPrintf("\n\n\nUsage: <exec> filename\n");
    CkExit();
  }
  char* filename = msg->argv[1];
  delete msg;

  /* TODO: read from input */
  uint64_t chromaticNum=7;  
  mainProxy= thisProxy;

  /* Parse the file and populate the graph*/
  parseCommandLine(filename,  adjList_);
  vertices_ = adjList_.size();
  

  CkPrintf("From Mainchare\n");
  std::cout << adjList_;  
  std::cout << vertices_;  

  std::vector<vertex> iState(vertices_, chromaticNum);
  populateInitialState(iState);

  std::vector<int> colorsPoss = getNextConstraintVertex(iState);
  /* 1. Create |colorsPoss| copies of iState.
   * 2. Update color and neighbr of each copy
   * 3. Spawn new Node chare for each copy
   */

  CProxy_Node n = CProxy_Node::ckNew();
  n.testGraph(iState);

}

Main::Main(CkMigrateMessage* msg) {}

void Main::done() { 
  CkExit();
}


/*If case the graph is partially colored,
 *iState need to be modified in accordance with 
 *adjList_
 */
void Main::populateInitialState(std::vector<vertex>& iState) {

}


#include "Module.def.h" 
