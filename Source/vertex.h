#ifndef _VERTEX_H
#define _VERTEX_H
 
#include "Utils.h"
#include "pup_stl.h"

class vertex  {
  public:
    vertex(uint64_t chromaticNum=0) : possible_colors_(chromaticNum) {
      vertex_id_ = 0;           // Can be deleted
      color_ = -1;
      possible_colors_.set();   //set every bit to 1 
      is_onStack = false;
    }
    bool isColored() { return color_ >=0;}
    int getColor() { return color_; }
    void setColor(int c) { color_ = c; possible_colors_.reset(); }

    void removePossibleColor(int c){
        if(!isColored())
            possible_colors_.reset(c);
    }

    const boost::dynamic_bitset<> getPossibleColor(){
        return possible_colors_;
    }

    void set_is_onStack(bool v) {
      is_onStack  = v;
    }

    const bool get_is_onStack() {
      return is_onStack;
    }
    void pup(PUP::er &p){
      p|vertex_id_;
      p|color_;
      p|is_onStack;

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
};

#endif
