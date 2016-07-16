#ifndef ALGO_GRAPHS_GRAPH_HPP_
#define ALGO_GRAPHS_GRAPH_HPP_

#include <cstddef>
#include <unordered_map>
#include <unordered_set>
#include <utility>

template <typename Node_key, typename W>
struct Edge
{
	Node_key dst;
    W weight;
};

template <typename Node_key, typename W>
bool operator<(const Edge<Node_key, W>& lhs, const Edge<Node_key, W>& rhs) noexcept
{
	return lhs.weight < rhs.weight;
}

template <typename Node_key, typename W>
struct Undirected_edge
{
	Node_key vertex_1;
	Node_key vertex_2;
    W weight;
};

template <typename Node_key, typename W>
bool operator<(const Undirected_edge<Node_key, W>& lhs, const Undirected_edge<Node_key, W>& rhs) noexcept
{
	return lhs.weight < rhs.weight;
}

template <typename Node_key, typename W>
bool operator==(const Undirected_edge<Node_key, W>& lhs, const Undirected_edge<Node_key, W>& rhs) noexcept
{
	return (lhs.vertex_1 == rhs.vertex_1 && lhs.vertex_2 == rhs.vertex_2) ||
			(lhs.vertex_1 == rhs.vertex_2 && lhs.vertex_2 == rhs.vertex_1);
}

template <typename Node_key, typename W>
struct Undirected_edge_hash
{
	std::size_t operator()(const Undirected_edge<Node_key, W>& edge) const noexcept
	{
		return std::hash<Node_key>()(edge.vertex_1) + std::hash<Node_key>()(edge.vertex_2);
	}
};

template <typename Node_key, typename W>
class Graph
{
public:
    typedef Node_key Node_key_type;
    typedef W Weight_type;
    typedef Edge<Node_key, W> Edge_type;
    typedef Undirected_edge<Node_key, W> Undirected_edge_type;
    typedef std::unordered_set<Undirected_edge_type, Undirected_edge_hash<Node_key, W>> Edge_set;

    decltype(auto) edges(const Node_key& node)
    {
        return data_.equal_range(node);
    }
    decltype(auto) edges(const Node_key& node) const
    {
        return data_.equal_range(node);
    }
    Edge_set unique_edges() const;
	decltype(auto) cbegin() const {	return data_.cbegin(); }
	decltype(auto) cend() const	{ return data_.cend(); }
	decltype(auto) begin() const {	return data_.begin(); }
	decltype(auto) end() const	{ return data_.end(); }
    std::size_t num_vertices() const { return vertices_.size(); }
    std::size_t num_edges() const { return data_.size(); }
    void add_edge(const Node_key& vertex_1, const Node_key& vertex_2, const W& weight);
	bool contains_vertex(const Node_key&) const;
private:
    std::unordered_multimap<Node_key, Edge<Node_key, W>> data_;
	std::unordered_set<Node_key> vertices_;
};

template <typename Node_key, typename W>
void Graph<Node_key, W>::add_edge(const Node_key& vertex_1, const Node_key& vertex_2, const W& weight)
{
	data_.insert(std::make_pair(vertex_1, Edge<Node_key, W>{vertex_2, weight}));
	data_.insert(std::make_pair(vertex_2, Edge<Node_key, W>{vertex_1, weight}));
	vertices_.insert(vertex_1);
	vertices_.insert(vertex_2);
}

template <typename Node_key, typename W>
bool Graph<Node_key, W>::contains_vertex(const Node_key& node_key) const
{
	return vertices_.find(node_key) != vertices_.cend();
}

template <typename Node_key, typename W>
typename Graph<Node_key, W>::Edge_set Graph<Node_key, W>::unique_edges() const
{
	Edge_set set;
	for (auto it = cbegin(); it != cend(); ++it)
	{
		set.insert(Undirected_edge_type{it->first, it->second.dst, it->second.weight});
	}
	return set;
}

#endif
