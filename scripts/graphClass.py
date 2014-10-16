class graph:
  def __init__(self):
    """ init method """
    self._graphDict={}

  def vertices(self):
    """ return the vertices in the graph as a list """
    return list(self._graphDict.keys())

  def edges(self):
    """ return the edges in the graph as a list of tuples """
    return generateEdgeList()

  def generateEdgeList(self):
    """ generate the list of edges """
    edges=[]
    for vertex1 in self._graphDict.keys():
      for vertex2 in _graphDict[vertex1]:
        if vertex1 > vertex2:
          edges.append(edge)  # undirected graph. Prevent (a,b) and (b,a) from being added twice
    
    return edges

  def addVertex(self, v):
    """ add a new vertex to the graph """
    assert(v not in self._graphDict.keys()) # vertex already in graph?
    self._graphDict[v]=[]

  def addEdge(self, t):
    """ add a new edge to the graph """
    (vertex1, vertex2) = tuple(t)
    assert(vertex1 in self._graphDict.keys())
    assert(vertex2 in self._graphDict.keys()) # adding edge, but end vertices not in graph?
    if vertex2 in self._graphDict[vertex1]:   # duplicate edge. Return false
      return False

    self._graphDict[vertex1].append(vertex2)
    self._graphDict[vertex2].append(vertex1)
    return True

  def getAdjacencyList(self):
    return self._graphDict

  def printAdjacencyList(self):
    """ print the adjacency list """
    for vertex in self._graphDict.keys():
      s = str(vertex) + ": "
      for neighbor in self._graphDict[vertex]:
        s = s + str(neighbor) + " "
      print s
