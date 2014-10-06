
#ifndef GRAPH_H
#define GRAPH_H

#include <map>
#include <memory>
#include <vector>
#include <list>
#include <string>
#include <iostream>

/*
 * An adjacency list representation for a Graph.
 * The algorithms are based on "Introduction to Algorithms" - CLRS.
 * Note that this uses some C++0x features (shared_ptr, lambdas).
 *
 */

enum VertexColour { WHITE, GRAY, BLACK };

//-----------------------------------------------------------------------------
class Edge;
class Vertex;
typedef std::shared_ptr<Vertex> VPtr;
typedef std::list<VPtr> VList;
typedef std::map<VPtr, VList> AdjList;
typedef std::map<VPtr, int> DegreeMap;
typedef std::vector<VPtr> STree;
//-----------------------------------------------------------------------------

class Vertex
{
public:
    Vertex(const std::string& name) : name_(name) {}
    VertexColour colour() const { return colour_; }
    void colour(VertexColour colour) { colour_ = colour; }
    VPtr pred() const { return pred_; }
    void pred(VPtr p) { pred_ = p; }
    friend std::ostream& operator<<(std::ostream& os, const Vertex& u)
    {
        os << u.name_;
        return os;
    }
    bool operator<(const Vertex& rhs) const
    {
        return name_ < rhs.name_;
    }

private:
    std::string name_;
    std::shared_ptr<Edge> edge_;
    VertexColour colour_;
    VPtr pred_;
};

//-----------------------------------------------------------------------------

class Edge
{
public:
    Edge(VPtr& u, VPtr& v, int weight = 0) : u_(u), v_(v), weight_(weight) { }
    Edge(const VPtr& u, const VPtr& v) : u_(u), v_(v), weight_(0) { }
    const VPtr u() const { return u_; }
    const VPtr v() const { return v_; }
private:
    VPtr u_;
    VPtr v_;
    int weight_;
};

//-----------------------------------------------------------------------------

class Graph
{
public:
    Graph() : components_(0) {}
    ~Graph() {}
    friend std::ostream& operator<<(std::ostream& os, const Graph& graph);

    const  AdjList& adjList() const;
    void   bfs(VPtr& s);
    int    components() const { return components_; }
    void   components(int c) { components_ = c; }
    int    degree(const VPtr& u);
    void   dfs();
    void   dfsWithStack();
    void   insert(const Edge& edge);
    bool   isConnected(VPtr& u, VPtr& v, bool hamiltonian = false);
    void   remove(const Edge& edge);
    STree  searchTree() const { return searchTree_; }
    int    vertices() { return vertices_; }

private:
    void dfsVisit(const VPtr& u);
    bool isConnectedHelper(VPtr& u, VPtr& v);
    bool isConnectedHamiltonianHelper(VPtr& u, VPtr& v, int d);
    void insertHelper(const VPtr& u, const VPtr& v);

    AdjList   adjList_;
    int       vertices_;
    DegreeMap degreeMap_;
    int       components_;
    STree     searchTree_;

    Graph(const Graph& rhs);
    Graph& operator=(const Graph& rhs);
};

#endif /* GRAPH_H */


