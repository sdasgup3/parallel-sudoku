#include"Utils.h"
#include <fstream>
#include <iostream>
#include <assert.h>   
#include  <string>
#include "graph.h"
#include <sstream>


using namespace std;


void parseCommandLine(char* filename, Graph& graph,  VextexMap&  i2vMap)
{
  ifstream fin;
  fin.open(filename); // open a file
  if (!fin.good()) {
    cout << "Input file NOT found\n";
    exit(1);
  }
  
  string str(""); 

  for (; std::getline(fin, str);) {
    stringstream ss(str); 
    string startV, endV;

    if(ss.str() == "") {
      continue;
    }

    ss >> startV;
    ss >> endV;

    assert(startV != "" && endV != "" && "Error is File Format!!");

#ifdef DEBUG
    cout << startV << " " << endV << endl;
#endif    
    insertEdgeHelper(graph, i2vMap, startV, endV);

  }

}
