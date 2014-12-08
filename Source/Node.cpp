#include "graphColor.h"
#include "string"
#define CkIntbits (sizeof(int)*8)
#define CkPriobitsToInts(nBits)    ((nBits+CkIntbits-1)/CkIntbits)
extern CkGroupID counterGroup;

//#define DEBUG 

/* -----------------------------------------
 * This is the node state BEFORE the search is initiated.
 * 1. initialize node_state_ with adjList all uncolored
 * 2. Does pre-coloring
 * ---------------------------------------*/
Node::Node(bool isRoot, int n, CProxy_Node parent) :
  nodeID_("0"), parent_(parent), uncolored_num_(n), is_root_(isRoot),
  child_num_(0), child_finished_(0),
  child_succeed_(0), is_and_node_(false), parentBits(1), parentPtr(NULL)

{
  programStart = CkTimer();
  CkAssert(isRoot);  
  vertex v = vertex(chromaticNum_);
  node_state_ = std::vector<vertex>(vertices_, v);

  //preColor();
#ifdef  DEBUG 
  CkPrintf("After Precolor\n");
  printGraph();
#endif 

  int count = uncolored_num_;
  uncolored_num_ -= vertexRemoval();
  CkPrintf("Vertices removed by vertex removal in [MainChare] = %d\n", count-uncolored_num_);

#ifdef  DEBUG 
  CkPrintf("Vertex Removal\n");
  printGraph();
#endif 

  CProxy_counter(counterGroup).ckLocalBranch()->registerMe(nodeID_);
  run();
}

/* --------------------------------------------
 * Node constructor with two parameters
 * used to fire child node by state configured
 * ---------------------------------------------*/
Node::Node( std::vector<vertex> state, bool isRoot, int uncol, 
    CProxy_Node parent, std::string nodeID, UShort pBits, UInt* pPtr, int size) : 
  nodeID_(nodeID), parent_(parent), uncolored_num_(uncol), node_state_(state), 
  is_root_(isRoot), child_num_(0), child_finished_(0),
  child_succeed_(0), is_and_node_(false), parentBits(pBits), parentPtr(pPtr)
{
  uncolored_num_ -= vertexRemoval();

  bool canISpawn = CProxy_counter(counterGroup).ckLocalBranch()->registerMe(nodeID_);

  // group chare denied permission to spawn
  if(!canISpawn)
    return;

  run();
}

/*  For a vertex 'vertex', return the number of nodes
 *  those are uncolored and not pushed on stack.   
 */
int Node::getUncoloredNgbr(int vertex)
{
  std::list<int> ngbr = adjList_[vertex];
  int num = 0;
  for(std::list<int>:: const_iterator it = ngbr.begin(), jt = ngbr.end(); 
      it != jt ; it++) {
    if(false == node_state_[*it].isColored() && true == node_state_[*it].isOperationPermissible())
      num ++;
  }
  return num;
}

/*  Remove uncolored vertices if the number of 
 *  is possible colorings is more than its 
 *  uncolored (and undeleted ) nghbrs. Worklist based algorithm
 */

int Node::vertexRemoval()
{ 
  int vertexRemoved = 0;
  std::set<int> worklist;

  // populate the initial worklist
  for(auto& I:adjList_)
  {
    if(node_state_[I.first].isColored() || !node_state_[I.first].isOperationPermissible())
      continue;
    worklist.insert(I.first);
  }

  while(!worklist.empty())
  {
    int vertex = *(worklist.begin());
    worklist.erase(worklist.begin());

    boost::dynamic_bitset<> possColors = node_state_[vertex].getPossibleColor();    
    int possColorCount  = possColors.count();
    int uncoloredNgbr   = getUncoloredNgbr(vertex);
    if(possColorCount > uncoloredNgbr) {
      node_state_[vertex].set_is_onStack(true);
      deletedV.push(vertex);
      vertexRemoved ++;

      // add the neighbors to worklist
      for(auto& I:adjList_[vertex])
      {
        if(node_state_[I].isColored() || !node_state_[I].isOperationPermissible())
          continue;
        worklist.insert(I);
      }
    }
  } 

  //  return total number of vertices put on stack
  return vertexRemoved;
}

/*  Coloring clique (i.j,k).
 *  assign min possible color to i and update its ngh.
 *  Do the same for j and k.
 *  While we are doing clique coloring we are also doing 
 *  forced moves (using updateState) which will give
 *  further improvements.
 */
void Node::colorClique3(int i, int j, int k)
{
  if(false == node_state_[i].isColored()) {
    boost::dynamic_bitset<> i_possC = node_state_[i].getPossibleColor();
    size_t i_color = i_possC.find_first();
    CkAssert(i_color != boost::dynamic_bitset<>::npos);
    int verticesColored =  updateState(node_state_, i, i_color , true);
    uncolored_num_  -=  verticesColored;
  }

  if(false == node_state_[j].isColored()) {
    boost::dynamic_bitset<> j_possC = node_state_[j].getPossibleColor();
    size_t j_color = j_possC.find_first();
    CkAssert(j_color != boost::dynamic_bitset<>::npos);
    int verticesColored = updateState(node_state_, j, j_color , true);
    uncolored_num_ -= verticesColored;
  }

  if(false == node_state_[k].isColored()) {
    boost::dynamic_bitset<> k_possC = node_state_[k].getPossibleColor();
    size_t k_color = k_possC.find_first();
    CkAssert(k_color != boost::dynamic_bitset<>::npos);
    int verticesColored = updateState(node_state_, k, k_color, true );
    uncolored_num_ -= verticesColored;
  }
}

/*  Finds all the cliques of size 3
 *  for each edge (u,v):
 *    for each vertex w:
 *      if (v,w) is an edge and (w,u) is an edge:
 *        return true
 *  and colors them using a simple
 *  coloring algorithm.
 */
void Node::preColor() 
{
  for (AdjListType::const_iterator it = adjList_.begin(); it != adjList_.end(); ++it) {
    for(std::list<int>::const_iterator jt = it->second.begin(); jt != it->second.end(); jt++ ) {
      for (AdjListType::const_iterator kt = adjList_.begin(); kt != adjList_.end(); ++kt) {

        if(node_state_[(*it).first].isColored() && node_state_[(*jt)].isColored() 
            && node_state_[(*kt).first].isColored()) {
          continue;
        }
        std::list<int>:: const_iterator it_ik = std::find(it->second.begin(), it->second.end(), (*kt).first);  
        std::list<int>:: const_iterator it_kj = std::find(kt->second.begin(), kt->second.end(), (*jt));  

        if(it_ik != it->second.end() && it_kj != kt->second.end()) {
          colorClique3((*it).first, (*jt), (*kt).first);         
        }
      }
    }
  }

}


Node::Node (CkMigrateMessage*) 
{
}

/* ----------------------------------------------
 *  Return the index of next uncolored and unstacked vertex which
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
    if(false == node_state_[i].isColored() && true == node_state_[i].isOperationPermissible()) {
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
 *  possible colors, i.e. c1 > c2, such that if we color vIndex with
 *  c1 then the number of possible colorings of its neighbours 
 *  will be more than the number if vIndex is colored with c2. 
 *  Also, if for a color c, the nghbr of vIndex reduced to
 *  0 possible colors, then that c will not be considered.(Impossibility Testing)
 *  ----------------------------------------------
 */
pq_type Node::getValueOrderingOfColors(int vIndex) 
{
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
    bool impossColoring = false;
    int rank = 0 ;

    for(std::list<int>::const_iterator jt = neighbours.begin(); jt != neighbours.end(); jt++ ) {
      if(node_state_[*jt].isColored() || !node_state_[*jt].isOperationPermissible()) continue;
      boost::dynamic_bitset<> possibleColorOfNgb  = node_state_[*jt].getPossibleColor();
      int count = possibleColorOfNgb.test(c) ? possibleColorOfNgb.count() -1 : possibleColorOfNgb.count();
      if(0 == count) {
        impossColoring = true;
        break;
      }
      rank = rank +  count;
    }
    if(false == impossColoring) {
      priorityColors.push(std::pair<size_t,int>(c,rank));
    }
  } 

  return priorityColors;
}

/*---------------------------------------------
 * updates an input state
 * 1. Color vertex[vIndex] with color c
 * 2. Updates all its neighbors corresponding possible colors
 * 3. doForcedMove == true -> color the ngh nodes recursively
 *                    if their possibility is reduced to 1.
 * 4. Return the number of vertices colored in the process. 
 * --------------------------------------------*/
int Node::updateState(std::vector<vertex> & state, int vIndex, size_t c, bool doForcedMove){
  int verticesColored = 0;

  if(state[vIndex].isColored() || !state[vIndex].isOperationPermissible()){ 
    return 0;
  }

  state[vIndex].setColor(c);
  verticesColored ++;

  for(std::list<int>::iterator it=adjList_[vIndex].begin();
      it!=adjList_[vIndex].end(); it++){
    state[*it].removePossibleColor(c);
  }

  if(true == doForcedMove) {
    for(std::list<int>::iterator it=adjList_[vIndex].begin();
        it!=adjList_[vIndex].end(); it++){
      boost::dynamic_bitset<> possColor = state[(*it)].getPossibleColor();
      if(1 == possColor.count()) {
        verticesColored += updateState(state, (*it), possColor.find_first(), doForcedMove);
      }
    }
  }
  return verticesColored;
}

// valid coloring found. Print statistics (if any)
void Node::printStats(std::vector<vertex>& state, bool success)
{
  if(success)
    CkPrintf("Valid Coloring found. Hurray! Printing stats.\n");
  else
    CkPrintf("Failed to color\n");

  CkPrintf("Program time (s) = %f\n", programEnd-programStart);
  int count=0;
  for_each(state.begin(), state.end(), [&](vertex v){
      if(v.getStats("vertexRemoval_local"))
      count++;
      });
  CkPrintf("Vertices removed by [Vertex Removal Local] = %d\n", count);

  //reset
  count = 0;  

  for_each(state.begin(), state.end(), [&](vertex v){
      if(v.getStats("vertexRemoval_remote"))
      count++;
      });
  CkPrintf("Vertices removed by [Vertex Removal Remote] = %d\n", count);

  // print total chare count
  CProxy_counter grp(counterGroup);
  CProxy_counter(counterGroup).ckLocalBranch()->getTotalCount();
}
/*--------------------------------------------
 * color the remaining graph locally
 * return if finish or not
 * -----------------------------------------*/
void Node::colorLocally()
{
  // register leaf node
  CProxy_counter(counterGroup).ckLocalBranch()->registerMeLeaf();

#ifdef DEBUG
  char *id = new char[nodeID_.size()];
  strcpy(id, nodeID_.c_str());
  CkPrintf("Color locally in node [%s] with uncolored num=%d\n", id, uncolored_num_);
  delete [] id;
#endif

  if(0 == uncolored_num_) {
    mergeRemovedVerticesBack(deletedV, node_state_);

    if(is_root_) {
#ifdef DEBUG
      printGraph(true);
#endif
      CkPrintf("Vertex removal step at root removed all nodes\n");
      CkAssert(1 == isColoringValid(node_state_));

      CkExit();
    } else {
      parent_.finish(true, node_state_);
    }
    return;
  }

  bool wait = false;
  bool success = sequentialColoring(wait);

  // sequential coloring timed out. Return without sending any message to
  // parent. Sending of the response to the parent would be done in rerun()
  if(wait)
    return;

  if(success){
    mergeRemovedVerticesBack(deletedV, node_state_);

    if(is_root_){
      CkAssert(1 == isColoringValid(node_state_));
#ifdef DEBUG
      printGraph(true);
#endif

      CkExit();
    } else {
      parent_.finish(true, node_state_);
    }
  } else {
    if(is_root_){
      programEnd = CkTimer();
      printStats(node_state_, false);
      CkExit();
    } else {
      mergeRemovedVerticesBack(deletedV, node_state_);
      parent_.finish(false, node_state_);
    }
  }
}

bool Node::sequentialColoring(bool& wait)
{
  sequentialStart = CkTimer();
  // stackForSequential = Stack which holds stackNodes objects. Each stackNode
  // object represents a node of the state space search. stackNode class is
  // similar to the node class. Second item in the pair is the stack of vertices
  // that were removed (in order) leading upto this point in the state space
  // search
  stackForSequential.emplace(std::make_pair(stackNode(node_state_, uncolored_num_), std::stack<int>()));
  bool solutionFound = false; 

  // if a solution is found in sequentialColoringHelper function,
  // the vertices in node_state_ are updated (colored)
  sequentialColoringHelper(wait, solutionFound, node_state_);
  return solutionFound;
}

// non-recursive algorithm with adaptive grain size control
void Node::sequentialColoringHelper(bool& wait, bool& solutionFound, std::vector<vertex>& result)
{
  while(!stackForSequential.empty())
  {
    sequentialCurr = CkTimer();

    // Check if the sequential algorithm has run for 'timeout' seconds. If yes,
    // then return and call rerun() entry method. That would execute the
    // remaining stack.
    if(sequentialCurr - sequentialStart > timeout)
    {
#ifdef DEBUG
      CkPrintf("Timeout in sequential coloring. Stack size=%d\n", stackForSequential.size());
#endif
      thisProxy.rerun();
      wait = true;
      return;
    }

    stackNode curr_node_ = stackForSequential.top().first;
    std::stack<int> removedVertices = stackForSequential.top().second;
    stackForSequential.pop();  // remove from stack

    // vertex removal step
    curr_node_.uncolored_num_ -= curr_node_.vertexRemoval(removedVertices);

    // coloring found
    if(curr_node_.uncolored_num_==0)
    {
      solutionFound = true;
      curr_node_.mergeRemovedVerticesBack(removedVertices);
      result = curr_node_.node_state_;
      return;
    }

    int vIndex = curr_node_.getNextConstrainedVertex();
    CkAssert(vIndex!=-1);

    // Value Ordering
    // temporary stack to invert the priority queue ordering
    std::stack<stackNode> tmp;
    pq_type priorityColors = curr_node_.getValueOrderingOfColors(vIndex);
    while(!priorityColors.empty()){
      std::pair<int,int> p = priorityColors.top();
      priorityColors.pop();
      std::vector<vertex> new_state = curr_node_.node_state_;
      int verticesColored = curr_node_.updateState(new_state, vIndex, p.first, true);
      CkAssert(verticesColored >= 1);
      tmp.emplace(stackNode(new_state, curr_node_.uncolored_num_ - verticesColored));
    }

    while(!tmp.empty())
    {
      stackForSequential.push(std::make_pair(tmp.top(), removedVertices));
      tmp.pop();
    }
  }

}

/*-------------------------------------------
 * A Node coloring routine which does the following 
 *  - Find the next constrained vertex, vIndex  (variable Ordering)
 *  - Test if the `vIndex` vertex is colorable. Return false if not.
 *  - Find the colors possible for vIndex (Value Ordering of Colors)
 *    - The colors will be found in the priority order   
 *    - If for a particular color option, one of the ngb of
 *      vIndex is reduced to zero available colors, then dont consider that
 *      color. (Impossibility Testing)
 *  - Spawn a new node state in priority order for each possible  color.
 *    - In that Node state, mark the color of vIndex
 *    - Update the coloring info for ngbs of vIndex
 *    - If by coloring vIndex with a particular color
 *      one of its ngbs reduced to one color possibility,
 *      then color that vertex as well recursively. (Forced Move) 
 *   ----------------------------------------*/
void Node::colorRemotely(){

#ifdef DEBUG
  char *id = new char[nodeID_.size()];
  strcpy(id, nodeID_.c_str());
  CkPrintf("Color remotely in node [%s] with uncolored num=%d\n", id, uncolored_num_);
  delete [] id;
#endif

  //-----------------------------------------------
  //Detect Subgraphs
  //If have >=2 subgraphs, make this node and_node
  //and spawn children to color each subgraphs
  //----------------------------------------------
  //only if some vertices are removed, otherwise
  //no new subgraphs will be created
  if(doSubgraph && (!deletedV.empty() || is_root_)){
  boost::dynamic_bitset<> init_bitset(vertices_);
  init_bitset.set();
  //initialize the bitset by marking all removed vertices as 0
  //and existed vertices as 1
  CkAssert(node_state_.size()==vertices_);
  for(int i=0; i<vertices_; i++){
    //these vertices have been removed from current graph
    if(!node_state_[i].isOperationPermissible())
      init_bitset.reset(i);
  }
  pq_subgraph_type prioritySubgraphs;
  //detect subgraphs and create states correspondingly
  if(detectAndCreateSubgraphs( init_bitset, prioritySubgraphs ) ){

    //#ifdef DEUBG
    char * id = new char[nodeID_.size()+1];
    strcpy(id, nodeID_.c_str());
    CkPrintf("Found %d subgraphs in node[%s]\n", prioritySubgraphs.size(), id);
    delete [] id;
    //#endif

    is_and_node_ = true;

    int numChildrenStates = prioritySubgraphs.size();
    UShort childBits = _log(numChildrenStates);
    while( ! prioritySubgraphs.empty() ){
      boost::dynamic_bitset<> subgraph_bitset = prioritySubgraphs.top();
      prioritySubgraphs.pop();

      std::vector<vertex> new_state = node_state_;
      boost::dynamic_bitset<> remove_bitset = init_bitset ^ subgraph_bitset;
#ifdef DEBUG
      std::string s;
      boost::to_string(subgraph_bitset, s);
      char * bits = new char[s.size()+1];
      strcpy(bits, s.c_str());
      CkPrintf("subgraph bitset: %s\n", bits);
      delete [] bits;
#endif
      for(int i=0; i<remove_bitset.size(); i++){
        if(remove_bitset.test(i)){
          new_state[i].set_out_of_subgraph(true);
        }
      }


      if(doPriority){
        CkEntryOptions* opts = new CkEntryOptions();
        UShort newParentPrioBits; UInt* newParentPrioPtr;
        UInt newParentPrioPtrSize;
        getPriorityInfo(newParentPrioBits, newParentPrioPtr, newParentPrioPtrSize,
            parentBits, parentPtr, childBits, child_num_);
        opts->setPriority(newParentPrioBits, newParentPrioPtr);
        CProxy_Node::ckNew(new_state, false,
            subgraph_bitset.count(), thisProxy,
            nodeID_+std::to_string(child_num_),
            newParentPrioBits, newParentPrioPtr, newParentPrioPtrSize,
            CK_PE_ANY, opts);
        free(newParentPrioPtr);
      } else {
        CProxy_Node::ckNew(new_state, false,
            subgraph_bitset.count(), thisProxy,
            nodeID_+std::to_string(child_num_),
            0, NULL, 0);
      }
      child_num_++;
    }
    return;
  }
  }
  // -----------------------------------------
  // Following code deals with case is_and_node=false
  // -----------------------------------------

  int vIndex = this->getNextConstraintVertex();
  CkAssert(vIndex!=-1);

  boost::dynamic_bitset<> possibleColor = node_state_[vIndex].getPossibleColor();
  if(!possibleColor.any()){
    parent_.finish(false, node_state_);
    return;
  }

  pq_type priorityColors = getValueOrderingOfColors(vIndex);
  int numChildrenStates = priorityColors.size();
  UShort childBits = _log(numChildrenStates);

  while (! priorityColors.empty()) {

    /* priorityColors contains the vertex 
     *   colors (c1 > c2 > c3 ..) in the order governed by the valueOrdering.
     *   If we are not concerned about prioritization while
     *   firing chares, i.e. doPriority == false , we can just spawn a chare for each poped
     *   element of priorityColors. Else if (doPriority == true)
     *   and say priorityColors has elements c1: c2:c3 (such that c1 > c2 > c3)
     *   then we have to fire c1, c2 and c3 with priority Ptr00, Ptr01, Ptr10
     *   respectively, where Ptr is the priority bits for their parent.
     */
    std::pair<int,int> p =  priorityColors.top();
    priorityColors.pop();

    std::vector<vertex> new_state = node_state_;
    int verticesColored = updateState(new_state, vIndex, p.first, true);
    CkAssert(verticesColored >= 1);

    if(doPriority) {
      CkEntryOptions* opts = new CkEntryOptions ();
      UShort newParentPrioBits; UInt* newParentPrioPtr;
      UInt newParentPrioPtrSize;
      getPriorityInfo(newParentPrioBits, newParentPrioPtr, newParentPrioPtrSize, parentBits, parentPtr, childBits, child_num_);
      opts->setPriority(newParentPrioBits, newParentPrioPtr);

      CProxy_Node::ckNew(new_state, false, uncolored_num_- verticesColored, 
          thisProxy, nodeID_ + std::to_string(child_num_), newParentPrioBits, newParentPrioPtr, 
          newParentPrioPtrSize, CK_PE_ANY , opts);
      child_num_ ++;
      free(newParentPrioPtr);
    } else {
      CProxy_Node::ckNew(new_state, false, uncolored_num_- verticesColored, 
          thisProxy, nodeID_ + std::to_string(child_num_), 0, NULL, 0);
      child_num_ ++;
    }
  }
}

void Node::getPriorityInfo(UShort & newParentPrioBits, UInt* &newParentPrioPtr, UInt &newParentPrioPtrSize, 
    UShort &parentPrioBits, UInt* &parentPrioPtr, UShort &childPrioBits, UInt &childnum)
{
  if(NULL == parentPrioPtr) {
    CkAssert(childPrioBits <= CkIntbits);
    newParentPrioBits  = childPrioBits + parentPrioBits;
    newParentPrioPtr  = (UInt *)malloc(sizeof(int));
    *newParentPrioPtr = childnum << (8*sizeof(unsigned int) - newParentPrioBits);
    newParentPrioPtrSize = 1;
    return;
  }

  newParentPrioBits           = parentPrioBits + childPrioBits;
  UShort parentPrioWords      = CkPriobitsToInts(parentPrioBits);
  UShort newParentPrioWords   = CkPriobitsToInts(parentPrioBits + childPrioBits);
  int shiftbits = 0;

  if(newParentPrioWords == parentPrioWords) {
    newParentPrioPtr  = (UInt *)malloc(parentPrioWords*sizeof(int));
    for(int i=0; i<parentPrioWords; i++) {
      newParentPrioPtr[i] = parentPrioPtr[i];
    }
    newParentPrioPtrSize  = parentPrioWords;

    if((newParentPrioBits) % (8*sizeof(unsigned int)) != 0) {
      shiftbits = 8*sizeof(unsigned int) - (newParentPrioBits)%(8*sizeof(unsigned int));
    }
    newParentPrioPtr[parentPrioWords-1] = parentPrioPtr[parentPrioWords-1] | (childnum << shiftbits);

  } else if(newParentPrioWords > parentPrioWords) {
    /* have to append a new integer */
    newParentPrioPtr  = (UInt *)malloc(newParentPrioWords*sizeof(int));
    for(int i=0; i<parentPrioWords; i++) {
      newParentPrioPtr[i] = parentPrioPtr[i];
    }
    newParentPrioPtrSize  = newParentPrioWords;

    if(parentPrioBits % (8*sizeof(unsigned int)) == 0) {
      shiftbits = sizeof(unsigned int)*8 - childPrioBits;
      newParentPrioPtr[parentPrioWords] = (childnum << shiftbits);
    } else { /*higher bits are appended to the last integer and then use anothe new integer */
      int inusebits = 8*sizeof(unsigned int) - (parentPrioBits % ( 8*sizeof(unsigned int)));
      unsigned int higherbits =  childnum >> (childPrioBits - inusebits);
      newParentPrioPtr[parentPrioWords-1] = parentPrioPtr[parentPrioWords-1] | higherbits;
      /* lower bits are stored in new integer */
      newParentPrioPtr[parentPrioWords] = childnum << (8*sizeof(unsigned int) - childPrioBits + inusebits);
    }
  }
}

/* ---------------------------------------------------
 * called when receive child finish response
 * - merge the part sent by child
 * - respond to parent
 * - decide whether to wait for more child or not
 * -  if it's and node, success means all children succeed
 * -  if it's or node, success means at least one child succeed
 * -  if all children return, don't need to wait
 * -------------------------------------------------*/
bool Node::mergeToParent(bool res, std::vector<vertex> state)
{

  child_finished_ ++;
  if(res==true){
    child_succeed_++;
  }

  bool success = is_and_node_ ?
    child_succeed_==child_num_ :
    (child_succeed_!=0);

  bool finish  = (child_finished_ == child_num_); 
  finish  |= (is_and_node_ ?
      (child_succeed_!=child_finished_) :
      (child_succeed_!=0));

  if(is_and_node_){
    for(int i=0; i<vertices_; i++ ){
      //if the vertex is removed from the subgraph
      //don't need to merge back
      if(state[i].isOperationPermissible()){
        // if the vertex has been colored before
        // check whether they are matched or not
        if(node_state_[i].isColored()){
          CkAssert(node_state_[i].getColor()==state[i].getColor());
        }
        //if the color is assigned from children node
        //assign the color to current node_state_
      	updateState(node_state_, i, state[i].getColor(), false);
      }
    }
#ifdef DEBUG
    printGraph();
    CkPrintf("child_succeed=%d, success=%d, finish=%d\n",
    	child_succeed_, success, finish);
#endif
  } else {
    if(child_succeed_)
  	  node_state_ = state;
  }

  if(is_root_ && finish){
    if(success){
      mergeRemovedVerticesBack(deletedV, node_state_);
#ifdef DEBUG
      printGraph(true);
#endif
      CkAssert(1 == isColoringValid(node_state_));
      CkExit();
    } else {
      programEnd = CkTimer();
      printStats(node_state_, false);
      CkExit();
    }
  } else if(!is_root_ && finish){
    mergeRemovedVerticesBack(deletedV, node_state_);
    parent_.finish(success, node_state_);
  } 

  return !finish;
}

void Node::mergeRemovedVerticesBack(std::stack<int> deletedV, std::vector<vertex> &node_state_) {
  while(!deletedV.empty()) {
    int vertex = deletedV.top();
    deletedV.pop();
    node_state_[vertex].set_is_onStack(false);
    boost::dynamic_bitset<> possColor = node_state_[vertex].getPossibleColor();
    size_t c = possColor.find_first();
    CkAssert(c != boost::dynamic_bitset<>::npos);
#ifdef DEBUG
    CkPrintf("Popped vertex %d color %d\n", vertex,c);
#endif
    updateState(node_state_, vertex, c, false);
  }
}

/* --------------------------------------------
 * print out node_state_
 * -------------------------------------------*/
void Node::printGraph(bool final){
  if(final)
    CkPrintf("--Printing final graph %s--\n", is_root_?"from Root Chare":"from Non-root Chare");
  else
    CkPrintf("--Printing partial graph %s--\n", is_root_?"from Root Chare":"from Non-root Chare");

  for(int i=0; i<node_state_.size(); i++){
    CkPrintf("vertex[%d]:color[%d] ;\n", i, node_state_[i].getColor());
  }
  CkPrintf("\n-------------------------------\n");
}

/* --------------------------------------------
 * Checks if the reported coloring is valid.
 * Only the root chare should check validity of solution
 * -------------------------------------------*/
bool Node::isColoringValid(std::vector<vertex>& state)
{
  CkAssert(is_root_); 

  for (AdjListType::const_iterator it = adjList_.begin(); it != adjList_.end(); ++it) {

    int iColor = state[(*it).first].getColor();

    if(iColor >= chromaticNum_ || iColor == -1 ){
        CkPrintf("fail due to using invalid colors.\n");
    	return 0; 
    }

    for(std::list<int>::const_iterator jt = it->second.begin(); jt != it->second.end(); jt++ ) {
      int jColor = state[*jt].getColor();
      if(jColor >= chromaticNum_ || -1 == jColor || iColor == jColor){
         CkPrintf("fail due to %d and %d has same color\n", it->first, *jt);
      	 return 0; 
      }
    }

  }  

  programEnd = CkTimer();
  storeColoredGraph();
  printStats(state);
  return  1;
}

int Node::_log(int n)
{
  int _mylog = 0;
  for(n=n-1;n>0;n=n>>1)
  {
    _mylog++;
  }
  return _mylog;
}


//--------------------------------------------------------
//detectAndCreateSubgraphs
// - return true if it consists more than one subgraphs
//---------------------------------------------------------
bool Node::detectAndCreateSubgraphs(boost::dynamic_bitset<> init_bitset,
    pq_subgraph_type & prioritySubgraphs)
{
#ifdef DEBUG
  if(init_bitset.count()!=vertices_){
    std::string s;
    char * id = new char[nodeID_.size()+1];
    strcpy(id, nodeID_.c_str());
    boost::to_string(init_bitset, s);
    char * bits = new char[s.size()+1];
    strcpy(bits, s.c_str());
    CkPrintf("in node [%s] detect subgraphs %s\n", id, bits);
    delete [] id;
    delete [] bits;
  }
#endif
  //keep track of vertices haven't been assigned to any subgraph
  boost::dynamic_bitset<> work_bitset(init_bitset);
  //record vertices in current computing subgraph
  boost::dynamic_bitset<> subgraph_bitset(vertices_);
  std::list<int> worklist;

  while(work_bitset.any()){
    //start working on getting a new subgraph
    subgraph_bitset.reset();
    worklist.clear();
    //get an unremoved and haven't considered vertex
    int first_bit = 0;
    while(!work_bitset.test(first_bit))
      first_bit++;
    //add it to current subgraph
    subgraph_bitset.set(first_bit);
    work_bitset.reset(first_bit);
    worklist.push_back(first_bit);

    do{
      //add its neighbors to subgraph iteratively
      int i = worklist.front();
      worklist.pop_front();
      for( int neighbor_vertex_index : adjList_[i]){
        if(work_bitset.test(neighbor_vertex_index)){
          subgraph_bitset.set(neighbor_vertex_index);
          work_bitset.reset(neighbor_vertex_index);
          worklist.push_back(neighbor_vertex_index);
        }
      }
    }while(!worklist.empty());

    prioritySubgraphs.push(subgraph_bitset);
  }

  return prioritySubgraphs.size()>1  ;
}

// stores the colored graph to a file which can be read by a python script to
// produce a png
void Node::storeColoredGraph()
{
  // should only be called by root chare once a valid coloring is found
  CkAssert(is_root_);

  std::ofstream myfile;
  myfile.open("coloredGraph.txt");

  // dump the adjacency list
  myfile << adjList_;

  // dump the coloring
  for(int i=0; i<node_state_.size(); i++)
    myfile << i << "-" << node_state_[i].getColor() << "\n";

  myfile.close();
}

void Node::rerun()
{
  bool wait = false;
  bool solutionFound = false;
  sequentialStart = CkTimer();
  sequentialColoringHelper(wait, solutionFound, node_state_);

  if(wait)
    return;

  if(solutionFound){
    mergeRemovedVerticesBack(deletedV, node_state_);

    if(is_root_){
      CkAssert(1 == isColoringValid(node_state_));
#ifdef DEBUG
      printGraph(true);
#endif

      CkExit();
    } else {
      parent_.finish(true, node_state_);
    }
  } else {
    if(is_root_){
      programEnd = CkTimer();
      printStats(node_state_, false);
      CkExit();
    } else {
      mergeRemovedVerticesBack(deletedV, node_state_);
      parent_.finish(false, node_state_);
    }
  }

}
