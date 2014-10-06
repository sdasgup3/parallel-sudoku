#ifndef _GRAPHCOLOR_H
#define _GRAPHCOLOR_H

#include <stdio.h>
#include <math.h>
#include  <string>
#include "Module.decl.h" 
#include "Utils.h"


/*readonly*/ 
extern CProxy_Main mainProxy;
extern CProxy_Node nodeProxy;

class Main : public CBase_Main {
  private:
    int checksum_before_balance;

  public:
  Main(CkArgMsg* msg);
  Main(CkMigrateMessage* msg);

  friend void parseCommandLine(char* );


  void done();
};

class Node: public CBase_Node {
  private:

  public:
  Node() ;
  Node (CkMigrateMessage*);
};


#endif
