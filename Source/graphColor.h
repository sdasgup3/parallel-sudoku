#ifndef _GRAPHCOLOR_H
#define _GRAPHCOLOR_H

#include "Utils.h"
#include "State.h"
#include "Module.decl.h" 

/*readonly*/ 
extern  CProxy_Main mainProxy;
extern  AdjListType adjList_;
extern  int vertices_;

class Main : public CBase_Main {
  private:

  public:
  Main(CkArgMsg* msg);
  Main(CkMigrateMessage* msg);

  friend void parseCommandLine(char*,AdjListType&, int&);
  friend std::vector<int> getNextConstraintVertex(std::vector<State>&);
  void done();
  void populateInitialState(std::vector<State>&);
};

class Node: public CBase_Node {
  private:

  public:
  Node() ;
  Node (CkMigrateMessage*);
  void testGraph(std::vector<State>& );
  friend std::vector<int> getNextConstraintVertex(std::vector<State>&);
};


#endif
