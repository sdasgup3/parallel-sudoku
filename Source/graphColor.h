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
extern bool doPriority;
extern bool baseline;
extern double timeout;

struct stackNode {
  std::vector<vertex> node_state_;
  int uncolored_num_;   //number of uncolored vertex
  stackNode(std::vector<vertex> state, int c): node_state_(state), uncolored_num_(c){}
  int getUncoloredNgbr(int);
  int vertexRemoval(std::stack<int>&);
  int getNextConstrainedVertex();
  pq_type getValueOrderingOfColors(int);
  int updateState(std::vector<vertex>&, int, size_t, bool);
  void mergeRemovedVerticesBack(std::stack<int>&);
};

class compareColorRank {
  public:
    bool operator()(std::pair<size_t, int> &p1, std::pair<size_t, int>  &p2)
    {
      return p1.second < p2.second;
    }
};

class compareSubgraphRank {
  public:
    bool operator()(boost::dynamic_bitset<> &p1, boost::dynamic_bitset<> &p2)
    {
      return p1.count() < p2.count();
    }
};


class priorityMsg : public CMessage_priorityMsg {
  public:
    UInt* parentPtr;
    UShort parentBits;
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
    //assume nodes out of subgraph and on stack -> out of scope
    //because when we try to color, we don't need to consider them
    boost::dynamic_bitset<> ignored_vertices;
    std::stack<int> deletedV;
    std::stack<std::pair<stackNode, std::stack<int> > > stackForSequential;
    bool is_root_;
    int vertexColored;    //The vertex colored in this node;Debug purpose
    CProxy_Node parent_;
    int uncolored_num_;   //number of uncolored vertex
    UInt child_num_;       //number of children this chare creates
                         //each element corresponds to a subgraph
    int child_finished_;  //current finished children
    int child_succeed_;
    bool is_and_node_;    //set to true if it requires all children reply
    UShort parentBits;          //Number of priority bits used 
    UInt* parentPtr;

    //timers
    double sequentialStart;
    double sequentialCurr;
    double programStart;
    double programEnd;

  public:
    // default constructor creats root node
    Node(bool isRoot, int n, CProxy_Node parent);
    Node(std::vector<vertex> state, bool isRoot, int n, CProxy_Node parent, 
        std::string parentID, UShort , UInt *, int);
    Node (CkMigrateMessage*);

    int getNextConstraintVertex();
    pq_type getValueOrderingOfColors(int);
    void preColor();
    int vertexRemoval();
    int vertexRemoval(int);
    void colorClique3(int i, int j, int k);
    int getUncoloredNgbr(int);
    void mergeRemovedVerticesBack(std::stack<int>, std::vector<vertex> &);
    void storeColoredGraph();

    // update a passed in state
    // by coloring vertex[vIndex] with color c
    int updateState(std::vector<vertex> & state, int vIndex, size_t c, bool doForcedMove);

    // print out graph colored states
    void printGraph(bool final=false);

    // color the remaining part of graph using sequential algorithm and respond
    // to parent with result
    void colorLocally();

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
    bool isColoringValid(std::vector<vertex>&);
    void printStats(std::vector<vertex>&, bool success=true);

    //detect subgraphs and create corresponding states
    //if only one graph existed, return false
    bool detectAndCreateSubgraphs(boost::dynamic_bitset<> init_bitset,
       pq_subgraph_type & subgraphs);

    void getPriorityInfo(UShort &, UInt* &, UInt&, UShort& , UInt*& , UShort& , UInt &);
    inline int _log(int n);

    bool sequentialColoring(bool&);
    void sequentialColoringHelper(bool&, bool&, std::vector<vertex>&);

    void rerun();
};

class DUMMYMSG : public CMessage_DUMMYMSG {
  public:
    int val;
};

class counter : public CBase_counter {
  private:
    int nCharesOnMyPe;
    int nLeafCharesOnMyPe;
    CkGroupID mygrp;
    int waitFor;
    CthThread threadId;
    int totalCount;
    bool acceptRegistration;

  public:
  counter(){
    mygrp = thisgroup;
    acceptRegistration = true;
    nCharesOnMyPe = 0;
    nLeafCharesOnMyPe = 0;
    waitFor = CkNumPes();
    totalCount = 0;
  }

  void registerMeLeaf(){
    nLeafCharesOnMyPe++;
    //if(nLeafCharesOnMyPe % 10 == 0)
    //  CkPrintf("[Heartbeat] Chares Spawned On PE %d = %d\n", CkMyPe(),nLeafCharesOnMyPe);
  }

  // return true if the group is still accepting new spawn requests from chares
  bool registerMe(){
    if(acceptRegistration)
    {
      nCharesOnMyPe++;
      if(nCharesOnMyPe % 1000 == 0)
        CkPrintf("[Heartbeat] Chares Spawned On PE %d = %d\n", CkMyPe(),nCharesOnMyPe);
      return true;
    }
    return false;
  }

  void sendCounts() {
    acceptRegistration = false;
    CkPrintf("Chares Spawned On PE %d = %d\n", CkMyPe(), nCharesOnMyPe);
    CkPrintf("Leaf Chares On PE %d = %d\n", CkMyPe(), nLeafCharesOnMyPe);
  }

  void getTotalCount() {
    acceptRegistration = false;
    CProxy_counter grp(mygrp);
    grp.sendCounts();
  }

};

#endif
