#ifndef _UTILS_H
#define _UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <assert.h>   
#include <map>   
#include <list>   
#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include <inttypes.h>

typedef std::map<int, std::list<int> > AdjListType;

/* forward declaration. TODO: Why are these functions made friend functions and kept here? */
class vertex;

void parseCommandLine(char* filename, AdjListType& adjList_);
std::ostream &operator<<(std::ostream &stream, const AdjListType& map);
void insertHelper(AdjListType& adjList_, const int& u, const int& v);

std::vector<int> getNextConstraintVertex(std::vector<vertex>&);

#endif
