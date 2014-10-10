#include"Utils.h"


using namespace std;


void parseCommandLine(char* filename, AdjListType& adjList_)
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

std::ostream &operator<<(std::ostream &stream, const AdjListType& map)
{
  for (AdjListType::const_iterator it = map.begin(); it != map.end(); ++it)
    {
      stream << (*it).first << " : " ;
      std::for_each(it->second.begin(), it->second.end(), [&](int v) { std::cout << v << " "; }); 
        std::cout << std::endl;
    }
  return stream;
}

/* Fir a given state finds the most constratint node and returns 
 * the possible colors in the order of pririty.
 */
std::vector<int> getNextConstraintVertex(std::vector<State>& state) 
{
  std::vector<int> colorsPoss(1);

  colorsPoss[0] = 9;

  return  colorsPoss;
}

