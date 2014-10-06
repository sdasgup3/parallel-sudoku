#include"Utils.h"
#include <fstream>
#include <iostream>
#include <assert.h>   
#include  <string>
#include "graph.h"
#include <sstream>


using namespace std;


void parseCommandLine(char* filename)
{
  ifstream fin;
  fin.open(filename); // open a file
  if (!fin.good()) {
    cout << "Input file NOT found\n";
    exit(1);
  }
  
  string str(""); 
  Graph graph;
  VextexMap i2vMap;

  for (; std::getline(fin, str);) {
    stringstream ss(str); 
    string startV, endV;

    if(ss.str() == "") {
      continue;
    }

    ss >> startV;
    ss >> endV;

    assert(startV != "" && endV != "" && "Error is File Format!!");

    insertEdgeHelper(graph, i2vMap, startV, endV);
  }

  cout << graph;  
}
