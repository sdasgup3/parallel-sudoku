#include "graphColor.h"
#include "string"
/* -----------------------------------------
 * Node Constructor with bool parameters
 * initialize node_state_ with adjList
 * all uncolored
 * ---------------------------------------*/
Node::Node(bool isRoot, int n, CProxy_Node parent) :
    parent_(parent), uncolored_num_(n), is_root_(isRoot)
{
    vertex v = vertex(chromaticNum_);
    node_state_ = std::vector<vertex>(vertices_, v);

    thisProxy.run();
}

/* --------------------------------------------
 * Node constructor with two parameters
 * used to fire child node by state configured
 * ---------------------------------------------*/
Node::Node(std::vector<vertex> state, bool isRoot, int n, CProxy_Node parent)
    : parent_(parent), uncolored_num_(n), node_state_(state), is_root_(isRoot)
{
    thisProxy.run();
}

Node::Node (CkMigrateMessage*) 
{
}

/* ----------------------------------------------
 * return the index of next constrained vertex
 * the vertex list will be got from its node_state
 *  For current implementation, we choose the first uncolored node
 *  ----------------------------------------------*/
int Node::getNextConstraintVertex(){

    //choose first uncolored node
    for(std::vector<vertex>::iterator it=node_state_.begin(); 
            it!=node_state_.end(); it++){
        if(it->isColored()==false){
            return std::distance(node_state_.begin(), it);
        }
    }
    // all finished coloring
    return -1;

}

/*---------------------------------------------
 * update a ppsed in state
 * by colorig vertex[vIndex] with color c
 * and update all its neighbors possible colors
 * --------------------------------------------*/
void Node::updateState(std::vector<vertex> & state, int vIndex, int c){
    state[vIndex].setColor(c);
    // remove possible color c from all the neighbors of vIndex
    for(std::list<int>::iterator it=adjList_[vIndex].begin();
            it!=adjList_[vIndex].end(); it++){
        state[*it].removePossibleColor(c);
    }
}

void Node::printGraph(){
    CkPrintf("\nFrom Chare - uncolored num = %d\n", uncolored_num_);
    for(int i=0; i<node_state_.size(); i++){
        CkPrintf("vertex[%d]:color[%d] ;\n", i, node_state_[i].getColor());
    }
    CkPrintf("\n-------------------------------\n");
}


void Node::testGraph(std::vector<vertex>& state) {
  
  CkPrintf("\nFrom Chare\n");
  std::cout << adjList_;  
  std::cout << "Number of vertices = "<<vertices_<<"\n";
  CkExit();
}

