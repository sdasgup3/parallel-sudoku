#!/usr/bin/env python
# script to produce colored graph as .png

import networkx as nx
import matplotlib.pyplot as plt 

adjList={}
vertexColor=[]
G=nx.Graph()

def produceImage():
  nx.draw(G, node_color=vertexColor, alpha=0.8)
  plt.savefig("graphColoring.png")
  plt.show()        

def createGraph():
  for line in open('coloredGraph.txt'):
    if line.__contains__(':'):
      vertex = int(line.split(':')[0].rstrip())
      neighbors = [int(x) for x in
        line.split(':')[1].lstrip().rstrip().rstrip('\n').split(' ')]
      adjList[vertex] = neighbors
    elif line.__contains__('-'):
      vertexColor.append(int(line.split('-')[1].rstrip('\n')))

  G.add_nodes_from(adjList.keys())
  listOfEdges=[]
  for k,v in adjList.iteritems():
    for vertex in v:
      listOfEdges.append((k,vertex))

  G.add_edges_from(listOfEdges)

if __name__=="__main__":
  createGraph()
  produceImage()
