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
#include <queue>
#include <stack>
#include <algorithm>
#include <boost/dynamic_bitset.hpp>
#include <inttypes.h>

// forward declaration
class compareColorRank;

typedef std::map<int, std::list<int> > AdjListType;
typedef std::priority_queue <std::pair<size_t,int>, std::vector<std::pair<size_t,int> >, compareColorRank> pq_type;
typedef unsigned int   UInt;
typedef unsigned short UShort;

/* forward declaration */ 
class vertex;

std::ostream &operator<<(std::ostream &stream, const AdjListType& map);
void insertHelper(AdjListType& adjList_, const int& u, const int& v);

#endif
