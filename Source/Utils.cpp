#include"Utils.h"
extern AdjListType adjList_;
extern int chromaticNum_;


using namespace std;

//---------functions below implemented in Main.h/cpp----------------
void parseInputFile(string filename)
{
  ifstream fin;
  fin.open(filename); // open a file
  if (!fin.good()) {
    cout << "Input file NOT found\n";
    exit(1);
  }
  
  string str(""); 
  string skip(""); 

  for (; std::getline(fin, str);) {

    stringstream ss(str); 
    if(std::string::npos != str.find("optimal_color"))  {

      char * cstr = new char [str.length()+1];
      std::strcpy (cstr, str.c_str());
      std::strtok(cstr,":");
      char* token = (char *)strtok(NULL,":");

      chromaticNum_ = atoi(token);
      continue;
    }
    if(std::string::npos != str.find("p edge"))  {
      break;
    }
  }

  int u , v ;
  for (; std::getline(fin, str);) {
    stringstream ss(str); 
    string startV, endV, token;

    if(ss.str() == "") {
      continue;
    }

    ss >> token >> startV >> endV ;

    assert(0 == token.compare("e") && "malformed graph input");
    assert(startV != "" && endV != "" && "Error is File Format!!");

    u = atoi(startV.c_str());
    v = atoi(endV.c_str());
    insertHelper(u - 1 , v - 1); // As the input graphs has vertices from 1 to ...
  }

}

void insertHelper(const int& u, const int& v)
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
      for(std::list<int>::const_iterator vit=it->second.begin();
              vit!=it->second.end(); vit++){
        stream << *vit << " ";
      }
      stream << "\n";
    }
  return stream;
}
