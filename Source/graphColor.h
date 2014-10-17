#ifndef _GRAPHCOLOR_H
#define _GRAPHCOLOR_H

#include "Module.decl.h" 

/*readonly*/ 
extern CProxy_Main mainProxy;
extern AdjListType adjList_;
extern int vertices_;
extern int chromaticNum_;

class Main : public CBase_Main {
  private:
    std::string filename, newGraph;
  
  public:
    Main(CkArgMsg* msg);
    Main(CkMigrateMessage* msg);

    void parseCommandLine(int argc, char **argv);
    void readDataFromPython(int argc, char **argv);
    
    void done();
    void populateInitialState(std::vector<vertex>&);
};

class Node: public CBase_Node {
  private:
    std::vector<vertex> node_state_;
    bool is_root_;
  public:
    // default constructor creats root node
    Node();
    Node(bool isRoot);
    Node(std::vector<vertex> state, bool isRoot);
    Node (CkMigrateMessage*);
    // ?? what this used for
    void testGraph(std::vector<vertex>& );
    // return the most constrained vertex id/index in vector
    int getNextConstraintVertex(std::vector<vertex>&);
};


#endif
