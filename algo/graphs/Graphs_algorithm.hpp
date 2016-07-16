#ifndef ALGO_GRAPHS_GRAPHS_ALGORITHM_HPP_
#define ALGO_GRAPHS_GRAPHS_ALGORITHM_HPP_

#include "Graph.hpp"
#include <queue>
#include <algorithm>

template <typename Node_key, typename W>
Graph<Node_key, W> prim_mst(const Graph<Node_key, W>& graph)
{
    Graph<Node_key, W> mst;
    typedef typename Graph<Node_key, W>::Undirected_edge_type Undirected_edge;
    std::priority_queue<Undirected_edge> queue;
    if (graph.num_vertices() == 0)
    {
    	return mst;
    }
    // Pick any node and put its edges in the queue
	auto start = graph.cbegin()->first;
	auto it = graph.edges(start);
	std::for_each(it.first,
			it.second,
			[&queue, &start] (const auto& e) { queue.push(Undirected_edge{start, e.second.dst, e.second.weight}); });
	
	while (mst.num_vertices() < graph.num_vertices())
	{
        // Pop lowest cost edge
		auto best = queue.top();
		queue.pop();

		if ((mst.contains_vertex(best.vertex_1) || mst.num_vertices() == 0) && !mst.contains_vertex(best.vertex_2))
		{
			mst.add_edge(best.vertex_1, best.vertex_2, best.weight);
			auto it = graph.edges(best.vertex_2);
			std::for_each(it.first, 
					it.second,
					[&queue, &best] (const auto& e)
							{ queue.push(Undirected_edge{best.vertex_2, e.second.dst, e.second.weight}); });
		}
	}
    return mst;
}

#endif
