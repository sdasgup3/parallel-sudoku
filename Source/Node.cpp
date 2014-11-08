#include "graphColor.h"
#include "string"
/* -----------------------------------------
 * Node Constructor with bool parameters
 * initialize node_state_ with adjList
 * all uncolored
 * ---------------------------------------*/
Node::Node(bool isRoot, int n, CProxy_Node parent) :
    parent_(parent), uncolored_num_(n), is_root_(isRoot),
    child_num_(0), child_finished_(0), 
    child_succeed_(0), is_and_node_(false)
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
    : parent_(parent), uncolored_num_(n), node_state_(state),
    is_root_(isRoot), child_num_(0), child_finished_(0),
    child_succeed_(0), is_and_node_(false)
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
 * update a passed in state
 * by colorig vertex[vIndex] with color c
 * and update all its neighbors corresponding possible colors
 * --------------------------------------------*/
void Node::updateState(std::vector<vertex> & state, int vIndex, int c){
    state[vIndex].setColor(c);
    // remove possible color c from all the neighbors of vIndex
    for(std::list<int>::iterator it=adjList_[vIndex].begin();
            it!=adjList_[vIndex].end(); it++){
        state[*it].removePossibleColor(c);
    }
}

/*--------------------------------------------
 * color the remaining graph locally
 * return if finish or not
 * TODO: when THREASHOLD!=1
 * for now the THREASHOLD is set to 1
 * so current implementation just color the 1 vertex
 * with the first avaialbe color
 * -----------------------------------------*/
void Node::colorLocally(){
    CkAssert(THRESHOLD==1);

    int vIndex = getNextConstraintVertex();
    CkAssert(vIndex!=-1);

    boost::dynamic_bitset<> possibleColor =
        node_state_[vIndex].getPossibleColor(); 
    if(possibleColor.any()){
        for(boost::dynamic_bitset<>::size_type c=0;
                c<possibleColor.size(); c++){
            if(possibleColor.test(c)){
                node_state_[vIndex].setColor(c);
                parent_.finish(true, node_state_);
            }
        }
    } else {
        parent_.finish(false, node_state_);
    }
}

/*-------------------------------------------
 * color remotely
 * TODO: maybe the function name is not that clear,
 * can change later
 * TODO: most heuristic methods are supposed to be put here
 * - for the choosed index and its possible colors
 * - try each one
 * - update its neighbors
 * - fire a child chare to color the remaining
 *   ----------------------------------------*/
void Node::colorRemotely(){

    if(is_and_node_){
        //TODO: call "Detect and Create Subgraph" functions here
        //TODO: fire children based on the subgraphs
        //return;
    }

    // -----------------------------------------
    // Following code deals with case is_and_node=false
    // -----------------------------------------


    //TODO: call "Pre-Color" Functions here
    //TODO: call "Vertex Remove" Function here
    //add variable in graphcolor.h file if needed


    //TODO: "Grainsize control" requires modify code below
    //get the vertex to color
    int vIndex = this->getNextConstraintVertex();

    boost::dynamic_bitset<> possibleColor(
            node_state_[vIndex].getPossibleColor());
    child_num_ = possibleColor.count();

    //test whehter colorable or not
    //return false if uncolorable
    //TODO: for each vertice, do "Impossible Test and Forced Moves"
    if(!possibleColor.any()){
        parent_.finish(false, node_state_);
        return;
    }

    //---------DEUBG USE BELOW------------
    //CkPrintf("In Chare[uncolor=%d],try to color vertex[%d] with %d colors\n",
    //        uncolored_num_, vIndex, possibleColor.count());
    //--------DEBUG USE ABOVE-------------

    // for each possible color
    // try it and create corresponding node state
    // to fire children
    for(boost::dynamic_bitset<>::size_type c=0;
            c<possibleColor.size(); c++){
        //if the color is not possible, skip it
        if(!possibleColor.test(c))
            continue;

        //get possible color c, and color it
        //update neighbor's possible_color_
        std::vector<vertex> new_state = node_state_;
        updateState(new_state, vIndex, c);
        //fire new child
        //TODO: "Value Numbering and Prioritization"
        CProxy_Node child = CProxy_Node::ckNew
            (new_state, false, uncolored_num_-1, thisProxy);
    }

}

/* ---------------------------------------------------
 * called when receive child finish response
 * - merge the part sent by child
 * - respond to parent
 * - decide whether to wait for more child or not
 * TODO:
 *  current implementation doesn't need to merge the graph
 *  but will be needed with later optimization
 * -------------------------------------------------*/
bool Node::mergeToParent(bool res, std::vector<vertex> state){
    
    child_finished_ ++;
    if(res==true){
        child_succeed_++;
    }

    bool success = is_and_node_ ?
                  //if it's and node, success means all children succeed
                  child_succeed_==child_num_ :
                  //if it's or node, success means at least one child succeed
                  (child_succeed_!=0);
    //if all children return, don't need to wait
    bool finish  = (child_finished_ == child_num_); 
    finish  |= is_and_node_ ?
               //if it's and node,
               //return success, terminate if finish=num
               //return fail, terminate if one fail
               (child_succeed_!=child_finished_) :
               //if it's or node,
               //return success, terminate one success
               //return fail, terminate all finish
               (child_succeed_!=0);

    if(is_and_node_){
        //TODO: call "Merge Subgraph" here
        //Attention: it's different from merge from stack
    }

    if(is_root_ && finish){
        // In Root and don't need to wait anymore
        if(success){
            // TODO: call "Merge From Stack" here
            // it's coupled with "Vertex Removal"
            node_state_ = state;
            printGraph();
            CkExit();
        } else {
            CkPrintf("Fail to color!\n");
            CkExit();
        }
    } else if(!is_root_ && finish){
        // In one child, successfully colored
        // TODO:: once it succeeds, it should notify other child chares
        // sharing the same parent to stop working
        // TODO:: add real merge function here
        node_state_ = state;
        parent_.finish(success, node_state_);
    } //else not finish
    //keep wait
    
    return !finish;
}

/* --------------------------------------------
 * print out node_state_
 * -------------------------------------------*/
void Node::printGraph(){
    if(is_root_)
        CkPrintf("\nFrom Root Chare");
    else 
        CkPrintf("\nFrom Chare");

    CkPrintf(" - uncolored num = %d\n", uncolored_num_);
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

