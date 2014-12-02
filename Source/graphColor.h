#ifndef _GRAPHCOLOR_H
#define _GRAPHCOLOR_H

#include "Module.decl.h"
#include "Utils.h"

/*readonly*/
extern CProxy_Main mainProxy;
extern AdjListType adjList_;
extern int vertices_;
extern int chromaticNum_;
extern int grainSize;

class compareColorRank {
  public:
    bool operator()(std::pair<size_t, int> &p1, std::pair<size_t, int>  &p2)
    {
      return p1.second < p2.second;
    }
};

class Main : public CBase_Main {
  private:
    std::string filename, newGraph;
  
  public:
    Main(CkArgMsg* msg);
    Main(CkMigrateMessage* msg);

    void parseCommandLine(int argc, char **argv);
    void readDataFromPython(int argc, char **argv);
    int getConservativeChromaticNum();
    void done();
    void populateInitialState(std::vector<vertex>&);
};

class Node: public CBase_Node {
  Node_SDAG_CODE

  private:
    std::string nodeID_;    // unique ID of the node
    std::vector<vertex> node_state_;
    std::stack<int> deletedV;
    bool is_root_;
    int vertexColored;    //The vertex colored in this node;Debug purpose
    CProxy_Node parent_;
    CkGroupID counterGroup;
    int uncolored_num_;   //number of uncolored vertex
    UInt child_num_;       //number of children this chare creates
                         //each element corresponds to a subgraph
    int child_finished_;  //current finished children
    int child_succeed_;
    bool is_and_node_;    //set to true if it requires all children reply
    UShort parentBits;          //Number of priority bits used 
    UInt* parentPtr;    

  public:
    // default constructor creats root node
    Node(bool isRoot, int n, CProxy_Node parent, CkGroupID);
    Node(std::vector<vertex> state, bool isRoot, int n, int, CProxy_Node parent, CkGroupID, std::string parentID, 
          UShort , UInt *, int);
    Node (CkMigrateMessage*);

    int getNextConstraintVertex();
    pq_type getValueOrderingOfColors(int);
    void preColor();
    int vertexRemoval(int);
    void colorClique3(int i, int j, int k);
    int getUncoloredNgbr(int);
    void mergeRemovedVerticesBack(std::stack<int>, std::vector<vertex> &);

    // update a passed in state
    // by coloring vertex[vIndex] with color c
    int updateState(std::vector<vertex> & state, int vIndex, size_t c, bool doForcedMove);

    // print out graph colored states
    void printGraph(bool final=false);

    // color the remaining part of graph using sequential algorithm and respond
    // to parent with result
    void sequentialColoring();

    // color remotely
    // choose a constreint vertex, color it
    // fire child chare to color remaining vertices
    void colorRemotely();

    // when receive its child response
    // current chare should decide how to merge with the received part
    // and how to respond to its parent
    // reutrn whether need to wait for another child or not
    bool mergeToParent(bool, std::vector<vertex>);

    //Checks if the reported coloring is valid. 
    bool isColoringValid(std::vector<vertex>);

    //detect subgraphs and create corresponding states
    //if only one graph existed, return false
    bool detectAndCreateSubgraphs(
       std::map<boost::dynamic_bitset<>, std::vector<vertex>> & subgraphs);

    void getPriorityInfo(UShort & newParentBits, UInt* &newParentPtr, UInt &newParentPtrSize, UShort& parentBits, UInt*& parentPtr, UShort& childBits, UInt &childnum);
    inline int _log(int n);

    bool solveBruteForce() ;
    bool solveBruteForceHelper(std::vector<vertex>& , std::vector<int> , int );

};

class DUMMYMSG : public CMessage_DUMMYMSG {
  public:
    int val;
};

class counter : public CBase_counter {
  private:
    int nCharesOnMyPe;
    CkGroupID mygrp;
    int waitFor;
    CthThread threadId;
    int totalCount;

  public:
  counter(){
    mygrp = thisgroup;
    nCharesOnMyPe = 0;
    waitFor = CkNumPes();
    totalCount = 0;
  }

  void registerMe(){
    nCharesOnMyPe++; 
  }

  void sendCounts() {
    CProxy_counter grp(mygrp);
    CkPrintf("num %d ", nCharesOnMyPe);
    grp[0].childCount(nCharesOnMyPe);
  }

  void childCount(int n) {
    totalCount += n;
    //waitFor--;
    //if (waitFor == 0)  
    //  if (threadId) { CthAwaken(threadId);}
  }


  DUMMYMSG* getTotalCount() {
    CProxy_counter grp(mygrp);
    grp.sendCounts();
    //threadId = CthSelf();
    //while (waitFor != 0)  CthSuspend(); 
    DUMMYMSG* msg = new DUMMYMSG();
    msg->val = totalCount;
    return msg;
  }


};

#endif
