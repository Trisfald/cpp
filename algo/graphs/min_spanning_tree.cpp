#include "Graphs_algorithm.hpp"
#include "Graph.hpp"
#include <iostream>

int main()
{
	Graph<unsigned, unsigned> graph;
	graph.add_edge(1, 2, 15);
	graph.add_edge(2, 3, 20);
	graph.add_edge(3, 4, 10);
	graph.add_edge(2, 4, 10);
	graph.add_edge(2, 5, 30);
	graph.add_edge(6, 5, 20);
	graph.add_edge(2, 6, 10);

	auto mst = prim_mst(graph);
	for (const auto& edge : mst.unique_edges())
	{
		std::cout << "edge from " << edge.vertex_1 << ", to " << edge.vertex_2 << "\n";
	}
}
