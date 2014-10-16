#undef _POSIX_C_SOURCE
#undef _XOPEN_SOURCE
#include <Python.h>
#include "boost/program_options.hpp"
#include "graphColor.h"

CProxy_Main mainProxy;
AdjListType adjList_;
int vertices_;

Main::Main(CkArgMsg* msg):newGraph("no") {

  /* Parse the file and populate the graph*/
  // parseInputFile(filename.c_str(),  adjList_);

  parseCommandLine(msg->argc, msg->argv);
  
  /* reads the adjacency list from python */
  readDataFromPython(msg->argc, msg->argv);
  vertices_ = adjList_.size();
  
  /* TODO: read from somewhere? */
  uint64_t chromaticNum=7;  
  mainProxy= thisProxy;

  CkPrintf("Mainchare constructor..\n");
  std::cout << adjList_;  
  std::cout << "Number of vertices = "<<vertices_<<"\n";  

  std::vector<vertex> iState(vertices_, chromaticNum);
  populateInitialState(iState);

  std::vector<int> colorsPoss = getNextConstraintVertex(iState);
  /* 1. Create |colorsPoss| copies of iState.
   * 2. Update color and neighbr of each copy
   * 3. Spawn new Node chare for each copy
   */

  CProxy_Node n = CProxy_Node::ckNew();
  n.testGraph(iState);
  delete msg;
}

Main::Main(CkMigrateMessage* msg) {}

void Main::done() { 
  CkExit();
}

void Main::parseCommandLine(int argc, char **argv)
{
  namespace po = boost::program_options;
  po::options_description desc("Allowed options");
  desc.add_options()
    ("newGraph", po::value<std::string>(),"Generate new graph from python (default=no)")
    ("filename", po::value<std::string>(),"Input file");

  po::variables_map vm;
  try
  {
    po::store(po::parse_command_line(argc, argv, desc), vm);
    if(vm.count("help"))
    {
      std::cout<<desc<<std::endl;
      CkExit();
    }

    // if parameter was passed in command line, assign it.
    if(vm.count("newGraph"))          
      newGraph.assign(vm["newGraph"].as<std::string>());
  }
  catch(po::error& e)
  {
    std::cout<<"ERROR:"<<e.what()<<std::endl;
    std::cout<<desc<<std::endl;
    CkExit();
  }
}

void Main::readDataFromPython(int argc, char **argv)
{
  PyObject *pModule, *pDict, *pFunc, *pGraph, *key, *value, *newDict;
  Py_ssize_t pos=0, listStart=0, listEnd=0;
  std::list<int> buff;
  
  // Initiaze the Python Interpreter
  Py_InitializeEx(0);

  // Load the module object
  if((pModule = PyImport_ImportModule("scripts")) == NULL)
  {
    printf("Unable to load module. Try setting PYTHONPATH to module location\n");
    assert(false);
  }

  // pDict is a borrowed reference 
  if((pDict = PyModule_GetDict(pModule)) == NULL)
  {
    printf("Unable to load module dictionary\n");
    assert(false);
  }

  // pFunc is also a borrowed reference 
  if((pFunc = PyDict_GetItemString(pDict, "getGraph")) == NULL)
  {
    printf("Function not found in module\n");
    assert(false);
  }
  else if(!PyCallable_Check(pFunc))
  {
    printf("Function not callable\n");
    assert(false);
  }
  
  if((newDict = PyDict_New()) == NULL)
  {
    printf("Failed to create new dictionary\n");
    assert(true);
  }

  // set-up the dictionary here with all the data that needs to be sent the to
  // python script
  PyDict_SetItemString(newDict, "newGraph", Py_BuildValue("s", newGraph.c_str()));

  // call function
  if((pGraph = PyObject_CallFunctionObjArgs(pFunc, newDict, NULL)) == NULL)
  {
    printf("Could not retrieve graph from python script\n");
    assert(true);
  }

  // iterate over the items in the python dictionary 
  // (key=vertex, value=list-of-adjacent-vertices)
  // key, value are borrowed references
  while (PyDict_Next(pGraph, &pos, &key, &value)) {
    listStart=0, listEnd = PyList_Size(value);
    for(int i=listStart; i<listEnd; i++)
      buff.push_back(PyInt_AsLong(PyList_GetItem(value, Py_ssize_t(i))));
     
    // populate the adjacency list of the main chare with data 
    adjList_.insert(std::map<int, std::list<int> >::value_type(PyInt_AsLong(key), buff));
    buff.clear();
   }

  // clean-up
  Py_DECREF(pModule);
  Py_DECREF(pGraph);
  Py_DECREF(newDict);

  // Finish the Python Interpreter
  Py_Finalize();
}

/*If case the graph is partially colored,
 *iState need to be modified in accordance with 
 *adjList_
 */
void Main::populateInitialState(std::vector<vertex>& iState) {

}


#include "Module.def.h" 
