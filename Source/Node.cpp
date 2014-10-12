#include "graphColor.h"


Node::Node() 
{
}

Node::Node (CkMigrateMessage*) 
{
};

void Node::testGraph(std::vector<vertex>& state) {
  
  CkPrintf("\nFrom Chare\n");
  std::cout << adjList_;  
  std::cout << vertices_;
  CkExit();
}

