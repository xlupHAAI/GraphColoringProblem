////////////////////////////////
/// usage : 1.	SDK for graph coloring solver.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef CN_HUST_SZX_NPBENCHMARK_GRAPH_COLORING_H
#define CN_HUST_SZX_NPBENCHMARK_GRAPH_COLORING_H


#include <array>
#include <vector>
#include <functional>
#include <unordered_set>

#define INF 0x7ffff000
#define NONE -1
#define MAXN 1005

namespace szx {

using NodeId = int;
using EdgeId = NodeId;
using ColorId = NodeId;

using Edge = std::array<NodeId, 2>; // undirected link.

struct GraphColoring {
	NodeId nodeNum;
	EdgeId edgeNum;
	ColorId colorNum;
	std::vector<Edge> edges;
}; 

using NodeColors = std::vector<ColorId>; // `NodeColors[n]` is the color of node `n`.

using Solution = NodeColors;
using ColorClass = std::unordered_set<NodeId>;


constexpr unsigned int nbmax = 40000;
constexpr double mu = 1.0;
constexpr int iter_cycle = 10;

struct critical_move {
	int benifit;
	NodeId transferee;
	ColorId dest;
	critical_move(int a, NodeId b, ColorId c) :benifit(a), transferee(b), dest(c) {  }
	critical_move() :benifit(INF) {  }
};

void solveGraphColoring(NodeColors& output, GraphColoring& input, std::function<long long()> restMilliSec, int seed);

}


#endif // CN_HUST_SZX_NPBENCHMARK_GRAPH_COLORING_H
