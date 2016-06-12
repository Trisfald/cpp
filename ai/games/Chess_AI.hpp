#ifndef AI_GAMES_CHESS_AI_HPP_
#define AI_GAMES_CHESS_AI_HPP_

#include <vector>
#include <random>
#include <unordered_set>
#include <utility>
#include <memory>
#include "Alpha_beta_decl.hpp"
#include "Chess_decl.hpp"

struct Chessboard;
struct Move_node;
typedef std::shared_ptr<Move_node> Move_node_ptr;

namespace detail
{

	template <typename Set>
	const typename Set::key_type& retrieve_or_insert(Set&, const typename Set::key_type&);

}

class Openings
{
    typedef std::unordered_set<Move_node_ptr> Data;
    typedef std::pair<Data::const_iterator, Data::const_iterator> Result;
public:
	Openings();

    template <typename C>
    Result get_available_moves(const C&, unsigned turn) const;
    Data::key_type::element_type& operator[](const Data::key_type& e)
	{
		return *detail::retrieve_or_insert(data_, e);
	}
private:
    Data data_;
};

class Move_generator
{
public:
	Move_generator();
    std::vector<Move> operator()(const Chessboard& board);
private:
	std::mt19937 re_;
	Openings openings_;
};

class Interrupt_tester
{
public:
    explicit Interrupt_tester(unsigned max_depth) : max_depth_(max_depth) {}
    Search_interrupt operator()(const Chessboard& board, unsigned depth) const;
private:
    unsigned max_depth_;
};

class Evaluator
{
public:
    explicit Evaluator(Team team) : team_(team) {}
    float operator()(const Chessboard& board, Search_interrupt) const;
private:
    Team team_;
};

struct Move_node
{
	typedef std::unordered_set<Move_node_ptr> Node_set;

	Move_node(Move move) : move(move) {}

	Node_set::key_type::element_type& operator[](const Node_set::key_type& e)
	{
		return *detail::retrieve_or_insert(nodes, e);
	}

	Move move;
	mutable Node_set nodes;
};

namespace std
{

	template <>
	struct hash<Move_node*>
	{
		std::size_t operator()(Move_node* node_ptr) const noexcept
		{
			auto x = node_ptr->move;
			return x.src.first + (x.src.second << 4) + (x.dst.first << 8) + (x.dst.second << 12);
		}
	};

}

bool operator==(const Move_node_ptr&, Move) noexcept;

bool operator==(const Move_node_ptr&, const Move_node_ptr&) noexcept;

template <typename C>
Openings::Result Openings::get_available_moves(const C& container, unsigned turn) const
{
    const Data* current = &data_;
    if (turn == 1)
    {
    	return {data_.cbegin(), data_.cend()};
    }
    auto it = data_.cbegin();
    for (unsigned i = 2; i <= turn; ++i)
    {
        if (i >= 2)
        {
            it = current->find(std::make_shared<Move_node>(Move_node(container[i - 2])));
        }

        if (it == current->cend())
        {
            return {data_.cend(), data_.cend()};
        }
        else
        {
            current = &(*it)->nodes;
        }
    }
    return {(*it)->nodes.cbegin(), (*it)->nodes.cend()};
}

namespace detail
{

	template <typename Set>
	const typename Set::key_type& retrieve_or_insert(Set& set, const typename Set::key_type& e)
	{
		auto it = set.find(e);
		if (it == set.cend())
		{
			auto ret = set.insert(e);
			return *ret.first;
		}
		return *it;
	}

	Move_node_ptr make_node(const std::string& cmd);

}

#endif
