#ifndef _GRAPHCOLOR_H
#define _GRAPHCOLOR_H

#include <stdio.h>
#include <math.h>
#include "Module.decl.h" //based on module name

/*readonly*/ 
extern CProxy_Main mainProxy;
extern CProxy_Node nodeProxy;

class Main : public CBase_Main {
  private:
    int checksum_before_balance;

  public:
  Main(CkArgMsg* msg);
  Main(CkMigrateMessage* msg);

  void done();
};

class Node: public CBase_Node {
  private:

  public:
  Node() ;
  Node (CkMigrateMessage*);
};

#endif
