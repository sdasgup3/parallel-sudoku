#ifndef _GRAPHCOLOR_H
#define _GRAPHCOLOR_H

#include "Module.decl.h" 

/*readonly*/ 
extern  CProxy_Main mainProxy;
extern  AdjListType adjList_;
extern  int vertices_;

class Main : public CBase_Main {
  private:
    std::string filename, newGraph;
  
  public:
    Main(CkArgMsg* msg);
    Main(CkMigrateMessage* msg);

    void parseCommandLine(int argc, char **argv);
    void readDataFromPython(int argc, char **argv);
    friend void parseInputFile(char*,AdjListType&, int&);
    friend std::vector<int> getNextConstraintVertex(std::vector<vertex>&);
    void done();
    void populateInitialState(std::vector<vertex>&);
};

class Node: public CBase_Node {
  private:

  public:
  Node() ;
  Node (CkMigrateMessage*);
  void testGraph(std::vector<vertex>& );
  friend std::vector<int> getNextConstraintVertex(std::vector<vertex>&);
};


#endif
