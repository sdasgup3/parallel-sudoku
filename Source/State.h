#ifndef _STATE_H
#define _STATE_H

//#include <bitset>
#include <vector>
#include "pup_stl.h"

const size_t bssz = size_t(9);


class State  {
  public:
    State() {
      vid = 0;  // Can be deleted
      isColored = false;
      colorsPossible  = -1; //All ones
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
    //std::bitset<bssz> colorsPossible;
    unsigned int  colorsPossible;
};

#endif
