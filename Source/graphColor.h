#ifndef _GRAPHCOLOR_H
#define _GRAPHCOLOR_H

#include <stdio.h>
#include <math.h>
#include  <string>
#include "Module.decl.h" 
#include "Utils.h"
#include "graph.h"


/*readonly*/ 
extern CProxy_Main mainProxy;

class Main : public CBase_Main {
  private:
    int checksum_before_balance;

  public:
  Main(CkArgMsg* msg);
  Main(CkMigrateMessage* msg);

  friend void parseCommandLine(char*,Graph&, VextexMap&);
  void done();
};

class Node: public CBase_Node {
  private:

  public:
  Node() ;
  Node (CkMigrateMessage*);
};


#endif
