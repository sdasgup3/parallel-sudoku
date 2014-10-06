#ifndef _STATE_H
#define _STATE_H

#include <bitset>

const size_t bssz = size_t(9);


class gNodeInfo {
  private:
    bool isColored;
    std::bitset<bssz> colorsPossible;

  public:
    gNodeInfo() {
      isColored = false;
      colorsPossible.set(true);
    }
    bool getIsColored() { return isColored;}
    void setIsColored(bool c) { isColored  = c;}

    //TO DO: member functions for accessing various info from colorsPossible

};

class State  {
  public:
    int colored_vid;        // The id of the vertex whose color is fixed by this state.
    std::vector<gNodeInfo> Info; 
};

#endif
