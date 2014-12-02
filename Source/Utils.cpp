#include"Utils.h"


using namespace std;

//---------functions below implemented in Main.h/cpp----------------
/*
void parseInputFile(char* filename, AdjListType& adjList_)
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

    int u = atoi(startV.c_str());
    int v = atoi(endV.c_str());
    insertHelper(adjList_, u, v);
    insertHelper(adjList_, v, u);
  }

}

void insertHelper(AdjListType& adjList_, const int& u, const int& v)
{
    AdjListType::iterator it = adjList_.find(u);
    if (it != adjList_.end()) {
      it->second.push_back(v);        
    } else {
      std::list<int> list;
      list.push_back(v);
      adjList_.insert(AdjListType::value_type(u, list));
    }
}
*/

std::ostream &operator<<(std::ostream &stream, const AdjListType& map)
{
  for (AdjListType::const_iterator it = map.begin(); it != map.end(); ++it)
    {
      stream << (*it).first << " : " ;
      for(std::list<int>::const_iterator vit=it->second.begin();
              vit!=it->second.end(); vit++){
        stream << *vit << " ";
      }
      stream << "\n";
    }
  return stream;
}



