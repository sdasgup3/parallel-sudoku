#ifndef _VERTEX_H
#define _VERTEX_H
  
#include "Utils.h"

class vertex  {
  public:
    vertex(uint64_t chromaticNum=0): possible_colors_(chromaticNum) {
      vertex_id_ = 0;  // Can be deleted
      is_colored_ = false;
      possible_colors_.set(); /* set every bit to 1 */
    }
    bool getIsColored() { return is_colored_;}
    void setIsColored(bool c) { is_colored_ = c;}
    void pup(PUP::er &p){
      p|vertex_id_;
      p|is_colored_;
      p|possible_colors_;
    }

  private:
    int vertex_id_;// The id of the vertex 
    bool is_colored_;
    boost::dynamic_bitset<> possible_colors_;
};

#endif
