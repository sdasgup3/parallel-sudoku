#ifndef _VERTEX_H
#define _VERTEX_H
  
#include "Utils.h"

class vertex  {
  public:
    vertex(uint64_t chromaticNum=0): colorsPossible(chromaticNum) {
      vid = 0;  // Can be deleted
      isColored = false;
      colorsPossible.set(); /* set every bit to 1 */
    }
    bool getIsColored() { return isColored;}
    void setIsColored(bool c) { isColored  = c;}
    void pup(PUP::er &p){
      p|vid;
      p|isColored;
      p|colorsPossible;
    }

  private:
    int vid;// The id of the vertex 
    bool isColored;
    boost::dynamic_bitset<> colorsPossible;
};

#endif
