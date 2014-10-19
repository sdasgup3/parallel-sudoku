#ifndef _VERTEX_H
#define _VERTEX_H
  
#include "Utils.h"

class vertex  {
  public:
    vertex(uint64_t chromaticNum=0): possible_colors_(chromaticNum) {
      vertex_id_ = 0;  // Can be deleted
      color_ = -1;
      possible_colors_.set(); /* set every bit to 1 */
    }
    bool isColored() { return color_ >=0;}
    int getColor() { return color_; }
    void setColor(int c) { color_ = c;}

    void removePossibleColor(int c){
        possible_colors_[c] = 0;
    }

    const boost::dynamic_bitset<> getPossibleColor(){
        return possible_colors_;
    }

    void pup(PUP::er &p){
      p|vertex_id_;
      p|color_;
      for(int i=0; i<possible_colors_.size(); i++){
          bool x=(bool)possible_colors_[i];
          p|x;
      }
    }

  private:
    int vertex_id_;// The id of the vertex 
    int color_; // if color<0, it means haven't been colored yet
    boost::dynamic_bitset<> possible_colors_;
};

#endif
