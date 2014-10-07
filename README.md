The graph coloring problem deals with assigning each vertex of the graph with a color such that the adjacent vertices have distinct colors. Many applications have been shown to be reducible to the graph coloring problem. Some of these include 1) register allocation in the back-end of a complier, and 2) scheduling of multiple tasks to resources under various constraints. Graph coloring is computationally hard. It is NP-complete to determine if a given graph admits k-coloring (except for k=1 or k=2).

State space search is a technique where successive configurations (a.k.a states) are explored until the state with desired property is found. The different states form a tree where child nodes are produced from the parent node by changing some part of the parent state. In typical problems, exploring the entire state space graph is impractical due to execution time and memory constraints. Application of heuristics has been shown to be effective in pruning the search space.

Parallel Graph Coloring
=========================
Large-scale systems with distributed memory are a natural fit to solving the
computationally complex graph coloring problem. There are two main
parallelization strategies, namely domain decomposition and state space
exploration. In domain decomposition, the graph is divided into sub-graphs, and
each of these is assigned to a processor. The sub-graph is colored locally at
each processor. The algorithm operates in time-steps, where at the end of each
time-step, the processors exchange coloring information with each other to
resolve the conflicts for the vertices on the sub-graph boundaries. Such an
approach is outlined in \cite{EuroPar2005}. State space exploration takes a
different approach to dividing work between processors. A \textit{state} in the
state space tree is a partially colored input graph, and child states are
produced from parent states by coloring one of the uncolored vertices. A
solution is found if one of the leaves if a legitimately colored input graph.
Each processor is responsible for exploration of a part of the state space
tree.

ParallelSudoku (An application of graph coloring)
===================================================
https://fenix.tecnico.ulisboa.pt/downloadFile/3779576294079/projSudoku.pdf
http://www.andrew.cmu.edu/user/hmhuang/project_template/finalreport.html
http://alitarhini.wordpress.com/2011/04/06/parallel-depth-first-sudoku-solver-algorithm/
