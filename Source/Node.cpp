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
Node::Node(bool isRoot, int n, CProxy_Node parent, CkGroupID g) :
  nodeID_("0"), parent_(parent), uncolored_num_(n), is_root_(isRoot),
  child_num_(0), child_finished_(0),  vertexColored(-1),
  child_succeed_(0), is_and_node_(false), parentBits(1), parentPtr(NULL), counterGroup(g)

{
  CkAssert(isRoot);  // nodeID=1, since this constructor is only called for root chare
  vertex v = vertex(chromaticNum_);
  node_state_ = std::vector<vertex>(vertices_, v);

  //preColor();
#ifdef  DEBUG 
  CkPrintf("After Precolor\n");
  printGraph();
#endif 
  
  for (AdjListType::const_iterator it = adjList_.begin(); it != adjList_.end(); ++it) {
    uncolored_num_ -= vertexRemoval((*it).first);
  } 

#ifdef  DEBUG 
  CkPrintf("Vertex Removal\n");
  printGraph();
#endif 

  CProxy_counter(counterGroup).ckLocalBranch()->registerMe();
  thisProxy.run();
}

/* --------------------------------------------
 * Node constructor with two parameters
 * used to fire child node by state configured
 * ---------------------------------------------*/
Node::Node( std::vector<vertex> state, bool isRoot, int uncol, int vColored,  
    CProxy_Node parent, CkGroupID g, std::string nodeID, UShort pBits, UInt* pPtr, int size) : 
  nodeID_(nodeID), parent_(parent), uncolored_num_(uncol), node_state_(state), 
  vertexColored(vColored), is_root_(isRoot), child_num_(0), child_finished_(0),
    child_succeed_(0), is_and_node_(false), parentBits(pBits), parentPtr(pPtr), counterGroup(g)

{
  for (AdjListType::const_iterator it = adjList_.begin(); it != adjList_.end(); ++it) {
    uncolored_num_ -= vertexRemoval((*it).first);
  }

  CProxy_counter(counterGroup).ckLocalBranch()->registerMe();
  thisProxy.run();
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
    if(false == node_state_[*it].isColored() && false == node_state_[*it].get_is_onStack()) {
      num ++;
    }
  }
  return num;
}

/*  Remove uncolored vertices recursively if the number of 
 *  is possible colorings is more than its 
 *  uncolored (and undeleted ) nghbrs.
 */
int Node::vertexRemoval(int vertex)
{
  int vertexRemoved = 0;
  if(node_state_[vertex].isColored() || node_state_[vertex].get_is_onStack()) {
    return 0;
  }
  boost::dynamic_bitset<> possColors = node_state_[vertex].getPossibleColor();    
  int possColorCount  = possColors.count();
  int uncoloredNgbr   = getUncoloredNgbr(vertex);
  if(possColorCount > uncoloredNgbr) {
    node_state_[vertex].set_is_onStack(true);
    deletedV.push(vertex);
    vertexRemoved ++;
    std::list<int> ngbr = adjList_[vertex];
    for(std::list<int>:: const_iterator it = ngbr.begin(), jt = ngbr.end(); 
        it != jt ; it++) {
      vertexRemoved += vertexRemoval(*it);
    }
  }
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
    if(false == node_state_[i].isColored() && false == node_state_[i].get_is_onStack()){
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
      if(node_state_[*jt].isColored() || node_state_[*jt].get_is_onStack()) continue;
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

  if(state[vIndex].isColored() || state[vIndex].get_is_onStack()) {
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

/*--------------------------------------------
 * color the remaining graph locally
 * return if finish or not
 * TODO: when THREASHOLD!=1
 * for now the THREASHOLD is set to 1
 * so current implementation just color the 1 vertex
 * with the first avaialbe color
 * -----------------------------------------*/
void Node::sequentialColoring()
{
  // 'vertex removal' and/or 'forced move' helped take out all the remaining
  // vertices, and we have none for the sequential algorithm
  if(0 == uncolored_num_) {
    mergeRemovedVerticesBack(deletedV, node_state_);


    if(is_root_) {
#ifdef DEBUG
      printGraph(true);
#endif
      CkPrintf("Sequential Coloring called from root. No parallelism\n");
      CkAssert(1 == isColoringValid(node_state_));

      /*
      CProxy_counter grp(counterGroup);
      DUMMYMSG* msg = grp[0].getTotalCount();
      int totalCharesSpawned = msg->val;
      CkPrintf("Total chares Spawned %d\n", totalCharesSpawned);
      delete msg;
      */

      CkExit();
    } else {
      parent_.finish(true, node_state_);
    }
    return;
  }

  if(solveBruteForce()){
    mergeRemovedVerticesBack(deletedV, node_state_);

    if(is_root_){
      CkAssert(1 == isColoringValid(node_state_));
#ifdef DEBUG
      printGraph(true);
#endif
      /*
      CProxy_counter grp(counterGroup);
      DUMMYMSG* msg = grp[0].getTotalCount();
      int totalCharesSpawned = msg->val;
      CkPrintf("Total chares Spawned %d\n", totalCharesSpawned);
      delete msg;
      */

      CkExit();
    } else {
      parent_.finish(true, node_state_);
    }
  } else {
    if(is_root_){
      CkPrintf("Fail to color!\n");
      CkExit();
    } else {
      mergeRemovedVerticesBack(deletedV, node_state_);
      parent_.finish(false, node_state_);
    }
  }
}

/*-------------------------------------------
 * At the leaf state when uncolored_num_ <= THRESHOLD
 *  we will try to color the graph using a brute force
 *  strategy.
 *   ----------------------------------------*/
bool Node::solveBruteForceHelper(std::vector<vertex>& state, std::vector<int> uncoloredIndices, int index )
{
  if(index == uncoloredIndices.size()) {
    return true;
  }
  boost::dynamic_bitset<> possibleColor = state[uncoloredIndices[index]].getPossibleColor(); 
  for(boost::dynamic_bitset<>::size_type c=0; c<possibleColor.size(); c++){
    if(true == possibleColor.test(c)) {
      std::vector<vertex> tempState = state;
      updateState(tempState, uncoloredIndices[index], c, false);
      if(true == solveBruteForceHelper(tempState, uncoloredIndices, index + 1)) {
        state = tempState;
        return true;
      }
    }
  }
  return false;
}

bool Node::solveBruteForce() 
{
  std::vector<int> uncoloredIndices;
  for(int  i = 0 ; i < node_state_.size(); i++){
    if(false == node_state_[i].isColored() && false == node_state_[i].get_is_onStack()){
      uncoloredIndices.push_back(i);
    }
  }
  return solveBruteForceHelper(node_state_, uncoloredIndices, 0);
}

/*-------------------------------------------
 * A Node coloring routine which does the following 
 *  - Find the next constrained vertex, vIndex  (variable Ordering)
 *  - Test if the `vIndex` vertex is colorable. Return false if not.
 *  - Find the colors possible for vIndex (Value Ordering of Colors)
 *    - The colors will be found in the priority order   
 *    - If for a perticular color option, one of the ngb of
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

  if(is_and_node_){
    //TODO: call "Detect and Create Subgraph" functions here
    //TODO: fire children based on the subgraphs
    //return;
  }

  // -----------------------------------------
  // Following code deals with case is_and_node=false
  // -----------------------------------------
  //TODO: "Grainsize control" requires modify code below

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

    std::pair<int,int> p =  priorityColors.top();
    priorityColors.pop();

    std::vector<vertex> new_state = node_state_;
    CkEntryOptions* opts = new CkEntryOptions ();
    UShort newParentPrioBits; UInt* newParentPrioPtr;
    UInt newParentPrioPtrSize;
    getPriorityInfo(newParentPrioBits, newParentPrioPtr, newParentPrioPtrSize, parentBits, parentPtr, childBits, child_num_);
    opts->setPriority(newParentPrioBits, newParentPrioPtr);

    int verticesColored = updateState(new_state, vIndex, p.first, true);

    CProxy_Node child = CProxy_Node::ckNew(new_state, false, uncolored_num_- verticesColored, 
        vIndex, thisProxy, counterGroup, nodeID_ + std::to_string(child_num_), newParentPrioBits, newParentPrioPtr, newParentPrioPtrSize, 
        CK_PE_ANY , opts);
    child_num_ ++;
    free(newParentPrioPtr);
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
    if(success){
      node_state_ = state;
      mergeRemovedVerticesBack(deletedV, node_state_);
      CkAssert(1 == isColoringValid(node_state_));
#ifdef DEBUG
      printGraph(true);
#endif

      /*
      CProxy_counter grp(counterGroup);
      DUMMYMSG* msg = grp[0].getTotalCount();
      int totalCharesSpawned = msg->val;
      CkPrintf("Total chares Spawned %d\n", totalCharesSpawned);
      delete msg;
      */

      CkExit();
    } else {
      CkPrintf("Fail to color!\n");
      CkExit();
    }
  } else if(!is_root_ && finish){
    // In one child, successfully colored
    // TODO:: once it succeeds, it should notify other child chares
    // sharing the same parent to stop working
    node_state_ = state;
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
 * -------------------------------------------*/
bool Node::isColoringValid(std::vector<vertex> state)
{
  // Only the root chare should check validity of solution
  CkAssert(is_root_); 

  for (AdjListType::const_iterator it = adjList_.begin(); it != adjList_.end(); ++it) {

    int iColor = state[(*it).first].getColor();

    if(iColor >= chromaticNum_ || iColor == -1 ) return 0; 

    for(std::list<int>::const_iterator jt = it->second.begin(); jt != it->second.end(); jt++ ) {
      int jColor = state[*jt].getColor();
      if(jColor >= chromaticNum_ || -1 == jColor || iColor == jColor) return 0; 
    }

  }  

  // valid coloring! Print statistics (if any)
  CkPrintf("Valid Coloring found. Hurray! Printing stats.\n");
  int vertexRemovalEfficiency = 0;
  for_each(state.begin(), state.end(), [&](vertex v){
      if(v._stat_vertexRemoval)
        vertexRemovalEfficiency++;
      });
  
  CkPrintf("Vertices removed by [Vertex Removal] = %d\n", vertexRemovalEfficiency);
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
