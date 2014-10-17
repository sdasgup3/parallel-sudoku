#include "graphColor.h"

/*--------------------------------------------
 * Default Node constructor
 * call Node(true)
 * ----------------------------------------*/
Node::Node() 
{
    this(true);    
}

/* -----------------------------------------
 * Node Constructor with bool parameters
 * initialize node_state_ with adjList
 * all uncolored
 * ---------------------------------------*/
Node::Node(bool isRoot) : is_root_(isRoot)
{
    node_state_ = std::vector<vertex>(vertices_, chromaticNum_);
    thisProxy.run();
}

/* --------------------------------------------
 * Node constructor with two parameters
 * used to fire child node by state configured
 * ---------------------------------------------*/
Node::Node(std::vector<vertex> state, bool isRoot)
    : node_state_(state), is_root_(isRoot)
{
    thisProxy.run();
}

Node::Node (CkMigrateMessage*) 
{
}

void Node::testGraph(std::vector<vertex>& state) {
  
  CkPrintf("\nFrom Chare\n");
  std::cout << adjList_;  
  std::cout << "Number of vertices = "<<vertices_<<"\n";
  CkExit();
}

