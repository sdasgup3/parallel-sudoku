import random, json, os, ast
from graphClass import graph
      
_vertices = 20
_edgeDensity = 20 # _edges/_vertices [reference: https://hal.inria.fr/file/index/docid/91354/filename/BELIV_2006_Melancon_Final.pdf]

class graphGen:
  def __init__(self):
    """ init method """
    return None

  def generate(self, incoming):
    """ creates the graph from specified parameters """
    # if a new graph is not desired and we have a JSON snapshot saved, return
    if incoming['newGraph'] == 'no' and os.path.isfile("./latestGraph.json"):
      print 'Retrieving old graph'
      data = json.load(open("latestGraph.json","r"))
      return dict((int(k), v) for k,v in data.iteritems())
    
    _edges = _vertices*_edgeDensity
    maxEdges = ((_vertices)*(_vertices-1))/2  
    if _edges > maxEdges:
      _edges = maxEdges/2   # in case n is small, and we exceed the maximum possible number of edges, we generate max/2

    g = graph()

    # populate vertices
    for i in xrange(_vertices):
      g.addVertex(i)

    # populate edges
    edgeCount = 0
    while edgeCount < _edges:
      vertex1 = random.randint(0,_vertices-1)   # generate random number. Ends inclusive
      vertex2 = random.randint(0,_vertices-1)  
      if vertex1 == vertex2:
        continue
      if g.addEdge((vertex1, vertex2)):
        edgeCount = edgeCount + 1 

    #g.printAdjacencyList()
    
    d = g.getAdjacencyList()
    # save latest graph in JSON format 
    json.dump(d, open("latestGraph.json","w"), indent=4)
    return d

