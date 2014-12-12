#ifndef _VERTEX_H
#define _VERTEX_H

#include "Utils.h"
#include "pup_stl.h"

// structure to collect statistics
typedef struct {
  bool vertexRemoval_remote = false;
  bool vertexRemoval_local = false;
  // add more here..

  void pup(PUP::er &p){
    p|vertexRemoval_remote;
    p|vertexRemoval_local;
  }

}statistics;

class vertex  {
  public:
    vertex(uint64_t chromaticNum=0) : possible_colors_(chromaticNum) {
      vertex_id_ = 0;           // Can be deleted
      color_ = -1;
      possible_colors_.set();   //set every bit to 1 
      is_onStack = false;
      is_out_of_subgraph = false;
    }

    bool isColored() { return color_ >=0;}
    int getColor() { return color_; }
    void setColor(int c) { color_ = c; possible_colors_.reset(); }

    void removePossibleColor(int c)
    {
      CkAssert(c < possible_colors_.size());
      if(!isColored()) {
        possible_colors_.reset(c);
      }
    }

    const boost::dynamic_bitset<> getPossibleColor(){
      return possible_colors_;
    }

    void set_is_onStack(bool v, bool isLocal=false) {
      is_onStack  = v;
      if(v)
      {
        if(isLocal)
          _stats.vertexRemoval_local=true;
        else
          _stats.vertexRemoval_remote=true;
      }
    }

    void set_out_of_subgraph(bool v) {
      is_out_of_subgraph  = v;
    }

    const bool get_is_onStack() {
      return is_onStack;
    }

    const bool get_is_out_of_subgraph() {
      return is_out_of_subgraph;
    }

    const bool isOperationPermissible() {
      return (false == is_onStack && false == is_out_of_subgraph);
    }

    const bool getStats(std::string name)
    { 
      if(name.compare("vertexRemoval_local")==0)
        return _stats.vertexRemoval_local;

      if(name.compare("vertexRemoval_remote")==0)
        return _stats.vertexRemoval_remote;

      CkAssert(false); // unknown statistic
    }

    void pup(PUP::er &p){
      p|vertex_id_;
      p|color_;
      p|is_onStack;
      p|is_out_of_subgraph;
      p|_stats;

      if(p.isUnpacking()){
        std::string s; 
        p|s;
        possible_colors_ = boost::dynamic_bitset<>(s);
      } else {
        std::string s;
        boost::to_string(possible_colors_, s);
        p|s;
      }
    }

  private:
    int vertex_id_; // The id of the vertex 
    int color_;     // if color<0, it means haven't been colored yet
    boost::dynamic_bitset<> possible_colors_;
    bool is_onStack;
    bool is_out_of_subgraph;
    statistics _stats;
};

#endif
