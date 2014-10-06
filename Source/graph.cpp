#include <queue>
#include <stack>
#include <vector>
#include <iterator>
#include <algorithm>
#include "graph.h"
//#include "UnitTest++.h"

std::ostream& operator<<(std::ostream& os, const Graph& graph)
{
    const AdjList adjList = graph.adjList();
    AdjList::const_iterator it;
    for (it = adjList.begin(); it != adjList.end(); ++it) {
        os << *it->first << ": ";
        std::for_each(it->second.begin(), it->second.end(), [&](VPtr v) { std::cout << *v << " "; });
        std::cout << std::endl;
    }
    return os;
}

void Graph::insert(const Edge& edge)
{
    insertHelper(edge.u(), edge.v());
    insertHelper(edge.v(), edge.u());
}

void Graph::insertHelper(const VPtr& u, const VPtr& v)
{
    AdjList::iterator it = adjList_.find(u);
    if (it != adjList_.end()) {
        it->second.push_back(v);        
        vertices_++;
    } else {
        VList list;
        list.push_back(v);
        adjList_.insert(AdjList::value_type(u, list));
        vertices_ += 2;
    }
}

void Graph::remove(const Edge& edge)
{
    AdjList::iterator it = adjList_.find(edge.u());
    if (it != adjList_.end()) {
        VList list = it->second;
        if (!list.empty()) {
            list.remove(edge.v());
        } else {

        }
    }
}

const AdjList& Graph::adjList() const
{
    return adjList_;
}

void Graph::bfs(VPtr& s)
{
    // colour all the vertices white, to mark them as unvisited.
    AdjList::iterator it;
    for (it = adjList_.begin(); it != adjList_.end(); ++it) {
        for (VList::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
            if (s != *vit) {
                (*vit)->colour(WHITE);
            }
        }
    }
    s->colour(GRAY); // frontier between discovered and undiscovered vertices.
    std::queue<VPtr> q;
    q.push(s);
    while (!q.empty()) {
        VPtr u = q.front();
        AdjList::iterator it = adjList_.find(u);
        if (it != adjList_.end()) {
            for (VList::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
                if (WHITE == (*vit)->colour()) {
                    (*vit)->colour(GRAY);
                    q.push(*vit);
                }
            }
        }
        u->colour(BLACK);
        std::cout << *u << " ";
        q.pop();
    }
}

void Graph::dfsWithStack()
{
    // colour all the vertices white, to mark them as unvisited.
    AdjList::iterator it;
    for (it = adjList_.begin(); it != adjList_.end(); ++it) {
        it->first->colour(WHITE);
    }
    std::stack<VPtr> stack;
    stack.push(adjList_.begin()->first);
    while (!stack.empty()) {
        VPtr u = stack.top();
        if (u->colour() == WHITE) {
            std::cout << *u << " ";
            u->colour(GRAY);
            AdjList::iterator it = adjList_.find(u);
            if (it != adjList_.end()) {
                for (VList::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
                    if ((*vit)->colour() == WHITE) {
                        stack.push(*vit);
                    }
                }
            }
        } else {
            u->colour(BLACK);
            stack.pop();
        }
    }
}

void Graph::dfs()
{
    // colour all the vertices white, to mark them as unvisited.
    AdjList::iterator it;
    for (it = adjList_.begin(); it != adjList_.end(); ++it) {
        it->first->colour(WHITE);
    }
    for (it = adjList_.begin(); it != adjList_.end(); ++it) {
        if (it->first->colour() == WHITE) {
            dfsVisit(it->first);
            components_++;
        }
    }
}

void Graph::dfsVisit(const VPtr& u)
{
    searchTree_.push_back(u);
    u->colour(GRAY);
    AdjList::iterator it = adjList_.find(u);
    if (it != adjList_.end()) {
        for (VList::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
            if ((*vit)->colour() == WHITE) {
                (*vit)->pred(u);
                dfsVisit(*vit);
            }
        }
    }
    u->colour(BLACK);
}

bool Graph::isConnected(VPtr& u, VPtr& v, bool hamiltonian)
{
    AdjList::iterator it;
    for (it = adjList_.begin(); it != adjList_.end(); ++it) {
        it->first->colour(WHITE);
    }
    if (hamiltonian)
        return isConnectedHamiltonianHelper(u, v, vertices() - 1);
    else
        return isConnectedHelper(u, v);
}

bool Graph::isConnectedHelper(VPtr& u, VPtr& v)
{
    // do a depth first search.
    if (u == v)
        return true;
    u->colour(GRAY);
    AdjList::iterator it = adjList_.find(u);
    for (VList::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
        if ((*vit)->colour() == WHITE) {
            if (isConnectedHelper(*vit, v))
                return true;
        }
    }
    return false;
}

bool Graph::isConnectedHamiltonianHelper(VPtr& u, VPtr& v, int d)
{
    // do a depth first search.
    if (u == v)
        return (d == 0);
    u->colour(GRAY);
    AdjList::iterator it = adjList_.find(u);
    for (VList::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
        if ((*vit)->colour() == WHITE) {
            if (isConnectedHamiltonianHelper(*vit, v, d - 1))
                return true;
        }
    }
    u->colour(WHITE);
    return false;
}

/**
 * Return the degree of vertex u
 */
int Graph::degree(const VPtr& u)
{
    AdjList::iterator it;
    for (it = adjList_.begin(); it != adjList_.end(); ++it) {
        degreeMap_[it->first] = 0;
        for (VList::iterator vit = it->second.begin(); vit != it->second.end(); ++vit) {
            degreeMap_[it->first]++;
        }
    }
    DegreeMap::const_iterator dit = degreeMap_.find(u);
    if (dit != degreeMap_.end())
        return dit->second;
    else
        return 0;
}

void  insertEdgeHelper(Graph &g , VextexMap & i2vMap,  std::string u, std::string v)
{
  int int_u = atoi(u.c_str());
  int int_v = atoi(v.c_str());

  VPtr U, V;

  if ( i2vMap.find(int_u) == i2vMap.end() ) {
    U = VPtr(new Vertex(u));
    i2vMap[int_u] = U;
  } else {
    U = i2vMap[int_u];
  }

  if ( i2vMap.find(int_v) == i2vMap.end() ) {
    V = VPtr(new Vertex(v));
    i2vMap[int_v] = V;
  } else {
    V = i2vMap[int_v];
  }
  g.insert(Edge(U,V));
}

/*
int main()
{
 Graph graph;
    VPtr v1(new Vertex("1"));
    VPtr v2(new Vertex("2"));
    VPtr v3(new Vertex("3"));
    VPtr v4(new Vertex("4"));
    VPtr v5(new Vertex("5"));

    // Component 1
    graph.insert(Edge(v1, v2));
    graph.insert(Edge(v1, v3));
    graph.insert(Edge(v2, v4));
    graph.insert(Edge(v2, v5));
    graph.insert(Edge(v1, v4));
    graph.insert(Edge(v5, v3));

    std::cout << graph;
    return 0;
    }

*/
