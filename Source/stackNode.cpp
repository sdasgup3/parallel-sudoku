#include "graphColor.h"

int stackNode::getUncoloredNgbr(int vIndex)
{
  std::list<int> ngbr = adjList_[vIndex];
  int num = 0;
  for(std::list<int>:: const_iterator it = ngbr.begin(), jt = ngbr.end(); 
      it != jt ; it++) {
    if(false == node_state_[*it].isColored() && true == node_state_[*it].isOperationPermissible()) 
      num ++;
    
  }
  return num;
}

int stackNode::vertexRemoval(std::stack<int>& removedVertices)
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
      node_state_[vertex].set_is_onStack(true, true);
      removedVertices.push(vertex);
      vertexRemoved ++;

      // add the neighbors to worklist
      for(auto& I:adjList_[vertex])
      {
        if(node_state_[I].isColored() || !node_state_[I].isOperationPermissible())
          continue;
        worklist.insert(I);
      }
    }
  } // while

  //  return total number of vertices put on stack
  return vertexRemoved;
}

int stackNode::getNextConstrainedVertex(){

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

pq_type stackNode::getValueOrderingOfColors(int vIndex) 
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
         
int stackNode::updateState(std::vector<vertex> & state, int vIndex, size_t c, bool doForcedMove){
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

void stackNode::mergeRemovedVerticesBack(std::stack<int>& removedVertices) {
  while(!removedVertices.empty()) {
    int vertex = removedVertices.top();
    removedVertices.pop();
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
