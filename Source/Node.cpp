#include "graphColor.h"
#include "string"

/* -----------------------------------------
 * Node Constructor with bool parameters
 * initialize node_state_ with adjList
 * all uncolored
 * ---------------------------------------*/
Node::Node(bool isRoot, int n, CProxy_Node parent) :
    parent_(parent), uncolored_num_(n), is_root_(isRoot),
    child_num_(0), child_finished_(0),  vertexColored(-1),
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
Node::Node(std::vector<vertex> state, bool isRoot, int uncol, int vColored,  CProxy_Node parent)
    : parent_(parent), uncolored_num_(uncol), node_state_(state), vertexColored(vColored),
    is_root_(isRoot), child_num_(0), child_finished_(0),
    child_succeed_(0), is_and_node_(false)
{
    thisProxy.run();
}

Node::Node (CkMigrateMessage*) 
{
}

/* ----------------------------------------------
 *  Return the index of next uncolored vertex which
 *  is most constrained.
 *  This heuristic will choose a vertex with the
 *  fewest colors available to it. 
 * 
 *  Return: index of most constrained vertex which is
 *  Else -1;
 *  ----------------------------------------------
 */
int Node::getNextConstraintVertex(){

    int cVertex, cVextexPossColor; 

    cVertex = -1;
    cVextexPossColor = chromaticNum_ + 1;

    for(int  i = 0 ; i < node_state_.size(); i++){
      if(node_state_[i].isColored() == false){
        boost::dynamic_bitset<> possibleColor = node_state_[i].getPossibleColor(); 
        if(cVextexPossColor > possibleColor.count() ) {
          cVertex = i;
          cVextexPossColor = possibleColor.count() ;
        }
      }
    }

    return cVertex;
}

/* ----------------------------------------------
 *  For a vertex id 'vIndex', returns the ordering of 
 *  possible colors, i.e. c1 < c2, such that if we color vIndex with
 *  c1 then the number of possible colorings of its neighbours 
 *  will be more than the number if vIndex is colored with c2. 
 *  ----------------------------------------------
 */
pq_type Node::getValueOrderingOfColors(int vIndex) 
{
  int rank; 
  pq_type  priorityColors;

  boost::dynamic_bitset<> possibleColor = node_state_[vIndex].getPossibleColor();
  if(false == possibleColor.any()) {
    return priorityColors;
  }

  std::list<int> neighbours = adjList_[vIndex];

  for(boost::dynamic_bitset<>::size_type c=0; c<possibleColor.size(); c++){
    if(false == possibleColor.test(c)) {
      continue;
    }
    
    rank = 0 ;
    for(std::list<int>::const_iterator jt = neighbours.begin(); jt != neighbours.end(); jt++ ) {
      boost::dynamic_bitset<> possibleColorOfNgb  = node_state_[*jt].getPossibleColor();
      rank = rank +  possibleColorOfNgb.test(c) ? possibleColorOfNgb.count() -1 : possibleColorOfNgb.count();
    }
    priorityColors.push(std::pair<int,int>(c,rank));
  }
  return priorityColors;
}

/*---------------------------------------------
 * update a passed in state
 * by colorig vertex[vIndex] with color c
 * and update all its neighbors corresponding possible colors
 * --------------------------------------------*/
void Node::updateState(std::vector<vertex> & state, int vIndex, int c){
    state[vIndex].setColor(c);
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
    //Spawn a node chare in priority order for each possible  color.
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
    int vIndex = this->getNextConstraintVertex();
    CkAssert(vIndex!=-1);

    //Test if the `vIndex` vertex is colorable.
    //Return false if un-colorable
    boost::dynamic_bitset<> possibleColor = node_state_[vIndex].getPossibleColor();
    child_num_ = possibleColor.count();
    if(!possibleColor.any()){
        parent_.finish(false, node_state_);
        return;
    }

    pq_type priorityColors = getValueOrderingOfColors(vIndex);

    //TODO: for each vertice, do "Impossible Test and Forced Moves"

    while (! priorityColors.empty()) {

      std::pair<int,int> p =  priorityColors.top();
      priorityColors.pop();

      std::vector<vertex> new_state = node_state_;
      updateState(new_state, vIndex, p.first);
      CProxy_Node child = CProxy_Node::ckNew
            (new_state, false, uncolored_num_-1, vIndex, thisProxy);
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
bool Node::mergeToParent(bool res, std::vector<vertex> state)
{
    
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
            CkAssert(1 == isColoringValid(node_state_));
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

/* --------------------------------------------
 * Checks if the reported coloring is valid.
 * -------------------------------------------*/
bool Node::isColoringValid(std::vector<vertex> state)
{
  for (AdjListType::const_iterator it = adjList_.begin(); it != adjList_.end(); ++it) {

    int iColor = state[(*it).first].getColor();

    if(iColor >= chromaticNum_) return 0; 

    for(std::list<int>::const_iterator jt = it->second.begin(); jt != it->second.end(); jt++ ) {
      int jColor = state[*jt].getColor();
      if(jColor >= chromaticNum_) return 0; 
      if(iColor == jColor) return 0;
    }

  }  
  return  1;
}
