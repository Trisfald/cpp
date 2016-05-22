#ifndef AI_SEARCHING_A_STAR_HPP_
#define AI_SEARCHING_A_STAR_HPP_

#include <vector>
#include <unordered_set>
#include <memory>
#include <utility>
#include <algorithm>
#include <memory>
#include <queue>
#include <cstddef>
#include <set>
#include <limits>
#include <tuple>

template <typename State, typename Action> class A_star_node;
template <typename State, typename Action> using A_star_node_ptr = std::unique_ptr<A_star_node<State, Action>>;
template <typename State, typename Action> class Full_result;

template <typename State, typename Action> using IEA_star_node_ptr = std::shared_ptr<A_star_node<State, Action>>;

/**
 * @brief Standard imprementation of the A* algorithm
 *
 * @tparam Generator is a callable returning all successors of a state
 * @tparam Heuristic is a callable returning the estimated f_cost of going from a state to the goal
 */
template <typename State,
		typename Action,
		typename Generator,
		typename Heuristic,
		template <typename, typename> class Result_policy = Full_result>
class A_star_search : public Result_policy<State, Action>
{
public:
	enum class Result
	{
		failure, cutoff, success, iteration_cutoff
	};
	typedef State State_type;
	typedef std::pair<typename Result_policy<State, Action>::Result_type, Result> Result_type;
	A_star_search(const Generator& generator = Generator(), const Heuristic& heuristic = Heuristic())
	: generator_(generator), heuristic_(heuristic)
	{}
	Result_type operator()(State start, State goal, float max_cost = std::numeric_limits<float>::max()) const;
protected:
	Result_type failure() const
	{
		return std::make_pair(typename Result_policy<State, Action>::Result_type(), Result::failure);
	}
	Result_type cutoff() const
	{
		return std::make_pair(typename Result_policy<State, Action>::Result_type(), Result::cutoff);
	}
	Generator generator_;
	Heuristic heuristic_;
private:
	typedef A_star_node<State, Action> Node;
	typedef A_star_node_ptr<State, Action> Node_ptr;
};

/**
 * @brief Policy to return a list of nodes as result
 */
template <typename State, typename Action>
class Full_result
{
protected:
	using Result_type = std::unique_ptr<std::vector<A_star_node<State, Action>>>;
	~Full_result() = default;

	Result_type make_path(const A_star_node<State, Action>& node) const
	{
		auto result = std::make_unique<typename Result_type::element_type>();
		auto node_ptr = &node;
		while (node_ptr->parent != nullptr)
		{
			result->emplace_back(std::move(*node_ptr));
			node_ptr = result->back().parent;
		}
		std::reverse(result->begin(), result->end());
		return result;
	}
	template <typename Ptr>
	Result_type make_path(Ptr* fw_node, Ptr* bw_node) const
	{
		auto result = std::make_unique<typename Result_type::element_type>();
		if (fw_node != nullptr)
		{
			const Ptr* node_ptr = fw_node;
			while (node_ptr->parent != nullptr)
			{
				result->emplace_back(std::move(*node_ptr));
				node_ptr = result->back().parent;
			}
			std::reverse(result->begin(), result->end());
		}
		if (bw_node != nullptr)
		{
			// Skip first
			if (bw_node->parent != nullptr)
			{
				Action previous_action = bw_node->action.get_reverse();
				const Ptr* node_ptr = bw_node->parent;
				while (node_ptr != nullptr)
				{					
					result->emplace_back(std::move(*node_ptr));
					auto temp_action = result->back().action; 
					result->back().action = previous_action;
					node_ptr = result->back().parent;
					previous_action = temp_action.get_reverse();
				}
			}
		}
		return result;
	}
};

/**
 * @brief Policy to return only a list of actions as result
 */
template <typename State, typename Action>
class Action_result
{
protected:
	using Result_type = std::unique_ptr<std::vector<Action>>;
	~Action_result() = default;

	Result_type make_path(const A_star_node<State, Action>& node) const
	{
		auto result = std::make_unique<typename Result_type::element_type>();
		auto node_ptr = &node;
		while (node_ptr->parent != nullptr)
		{
			result->emplace_back(std::move(node_ptr->action));
			node_ptr = node_ptr->parent;
		}
		std::reverse(result->begin(), result->end());
		return result;
	}
	template <typename Ptr>
	Result_type make_path(Ptr* fw_node, Ptr* bw_node) const
	{
		auto result = std::make_unique<typename Result_type::element_type>();
		if (fw_node != nullptr)
		{
			const Ptr* node_ptr = fw_node;
			while (node_ptr->parent != nullptr)
			{
				result->emplace_back(std::move(node_ptr->action));
				node_ptr = node_ptr->parent;
			}
			std::reverse(result->begin(), result->end());
		}
		if (bw_node != nullptr)
		{
			// Skip first
			if (bw_node->parent != nullptr)
			{
				const Ptr* node_ptr = bw_node;
				while (node_ptr->parent != nullptr)
				{					
					result->emplace_back(std::move(node_ptr->action.get_reverse()));
					node_ptr = node_ptr->parent;
				}
			}
		}
		return result;
	}
};


namespace detail
{

	template <typename State, typename Action>
	struct Basic_node
	{
		Basic_node(float f_cost, float g_cost, const State& state, const Action& action)
		: f_cost(f_cost), g_cost(g_cost), state(state), action(action)
		{}
		Basic_node(float f_cost, float g_cost, State&& state, Action&& action)
		: f_cost(f_cost), g_cost(g_cost), state(std::move(state)), action(std::move(action))
		{}
		float f_cost;
		float g_cost;
		State state;
		Action action;
	};

	template <typename State, typename Action>
	inline bool operator==(const Basic_node<State, Action>lhs, const Basic_node<State, Action>& rhs) noexcept
	{
		return lhs.state == rhs.state;
	}

	template <typename State, typename Action>
	struct A_star_node_ptr_equality
	{
	    inline bool operator()(const A_star_node<State, Action>* lhs,
	    		const A_star_node<State, Action>* rhs) const noexcept
	    {
	        return *lhs == *rhs;
	    }
	};

	template <typename Node, typename Frontier, typename Frontier_set>
	inline void a_star_add_frontier(Node&& node, Frontier& frontier, Frontier_set& frontier_set)
	{
		frontier_set.insert(node.get());
		frontier.push(std::move(node));
	}

	template <typename Node, typename Frontier, typename Frontier_set>
	inline void a_star_erase_frontier(Node&& node, Frontier& frontier, Frontier_set& frontier_set)
	{
		frontier_set.erase(node.get());
		frontier.pop();
	}

	template <typename Frontier, typename Frontier_set>
	inline typename Frontier::value_type a_star_pop_frontier(Frontier& frontier, Frontier_set& frontier_set)
	{
		auto node_ptr = std::move(const_cast<typename Frontier::value_type&>(frontier.top()));
		frontier.pop();
		frontier_set.erase(node_ptr.get());
		return node_ptr;
	}

}

/**
 * A* node
 */
template <typename State, typename Action>
struct A_star_node : detail::Basic_node<State, Action>
{
	A_star_node(float f_cost, float g_cost, State state, Action action, const A_star_node* parent);
	A_star_node(const A_star_node&);
	A_star_node(A_star_node&&) noexcept;
	A_star_node& operator=(const A_star_node&) noexcept;

	template <typename Generator, typename Heuristic>
	std::vector<A_star_node_ptr<State, Action>>
	successors(const Generator& generator, const Heuristic& heuristic, const State& goal) const;

	const A_star_node* parent;
private:
	typedef detail::Basic_node<State, Action> Base;
};

template <typename State, typename Action>
A_star_node<State, Action>::A_star_node(float f_cost,
		float g_cost,
		State state,
		Action action,
		const A_star_node* parent)
: Base(f_cost, g_cost, state, action), parent(parent)
{}

template <typename State, typename Action>
A_star_node<State, Action>::A_star_node(const A_star_node& rhs)
: Base(rhs.f_cost, rhs.g_cost, rhs.state, rhs.action), parent(rhs.parent)
{}

template <typename State, typename Action>
A_star_node<State, Action>::A_star_node(A_star_node&& rhs) noexcept
: Base(rhs.f_cost, rhs.g_cost, std::move(rhs.state), std::move(rhs.action)),
		parent(rhs.parent)
{}

template <typename State, typename Action>
A_star_node<State, Action>& A_star_node<State, Action>::operator=(const A_star_node<State, Action>& rhs) noexcept
{
	this->f_cost = rhs.f_cost;
	this->g_cost = rhs.g_cost;
	this->state = rhs.state;
	this->action = rhs.action;
	parent = rhs.parent;
	return *this;
}

template <typename State, typename Action>
inline bool operator==(const A_star_node_ptr<State, Action>& lhs,
		const A_star_node_ptr<State, Action>& rhs) noexcept
{
	return *lhs == *rhs;
}

template <typename State, typename Action>
inline bool operator>(const A_star_node_ptr<State, Action>& lhs,
		const A_star_node_ptr<State, Action>& rhs) noexcept
{
	if (lhs->f_cost != rhs->f_cost)
	{
		return lhs->f_cost > rhs->f_cost;
	}
	return lhs->g_cost < rhs->g_cost;
}

template <typename State, typename Action>
template <typename Generator, typename Heuristic>
std::vector<A_star_node_ptr<State, Action>> A_star_node<State, Action>::successors(const Generator& generator,
		const Heuristic& heuristic,
		const State& goal) const
{
	std::vector<A_star_node_ptr<State, Action>> children;
	for (auto& successor : generator(this->state))
	{
		auto f_cost = this->g_cost + heuristic(std::get<0>(successor), goal);
		children.push_back(std::make_unique<A_star_node<State, Action>>(f_cost,
				this->g_cost + std::get<2>(successor),
				std::move(std::get<0>(successor)),
				std::move(std::get<1>(successor)),
				this));
	}
	return children;
}


namespace std
{

	template <>
	template <typename State, typename Action>
	struct hash<A_star_node<State, Action>*>
	{
		inline std::size_t operator()(const A_star_node<State, Action>* x) const
		{
			return std::hash<State>()(x->state);
		}
	};

}


template <typename State,
		typename Action,
		typename Generator,
		typename Heuristic,
		template <typename, typename> class Result_policy>
typename A_star_search<State, Action, Generator, Heuristic, Result_policy>::Result_type
A_star_search<State, Action, Generator, Heuristic, Result_policy>::operator()(State start,
		State goal,
		float max_cost) const
{
	using Frontier = std::priority_queue<Node_ptr,
			std::vector<Node_ptr>,
			std::greater<Node_ptr>>;
	using Frontier_set = std::unordered_set<Node*,
			std::hash<Node*>,
			detail::A_star_node_ptr_equality<State, Action>>;
	using Node_set = std::unordered_set<Node_ptr>;

	Frontier frontier;
	Frontier_set frontier_set;
	Node_set explored;
	bool cutoff_occurred = false;

	auto root_ptr = std::make_unique<Node>(this->heuristic_(start, goal), 0, start, Action(), nullptr);
	detail::a_star_add_frontier(std::move(root_ptr), frontier, frontier_set);
	while (true)
	{
		if (frontier.empty())
		{
			return cutoff_occurred ? cutoff() : failure();
		}

		auto node_ptr = detail::a_star_pop_frontier(frontier, frontier_set);
		if (node_ptr->state == goal)
		{
			return std::make_pair(std::move(Result_policy<State, Action>::make_path(*node_ptr)), Result::success);
		}

		auto insert_pair = explored.insert(std::move(node_ptr));
		if ((*insert_pair.first)->g_cost > max_cost)
		{
			cutoff_occurred = true;
			continue;
		}
		for (auto& successor_ptr : (*insert_pair.first)->successors(generator_, heuristic_, goal))
		{
			auto frontier_successor_it = frontier_set.find(successor_ptr.get());
			if (frontier_successor_it == frontier_set.end())
			{
				if (explored.find(successor_ptr) == explored.end())
				{
					detail::a_star_add_frontier(std::move(successor_ptr), frontier, frontier_set);
				}
			}
			else if ((*frontier_successor_it)->f_cost > successor_ptr->f_cost)
			{
				*(*frontier_successor_it) = *successor_ptr.get();
			}
		}
	}
}


/**
 * @brief IDA* is a modification of A* that trades performance for memory usage
 */
template <typename State,
		typename Action,
		typename Generator,
		typename Heuristic,
		template <typename, typename> class Result_policy = Full_result>
class IDA_star_search : public A_star_search<State, Action, Generator, Heuristic, Result_policy>
{
	typedef A_star_search<State, Action, Generator, Heuristic, Result_policy> Base;
public:
	using typename Base::State_type;
	using typename Base::Result_type;
	using typename Base::Result;
	using Base::Base;
	Result_type operator()(State start, State goal, float max_cost = std::numeric_limits<float>::max()) const;
protected:
	Result_type iteration_cutoff() const
	{
		return std::make_pair(typename Result_policy<State, Action>::Result_type(), Result::iteration_cutoff);
	}
private:
	typedef A_star_node<State, Action> Node;
	typedef A_star_node_ptr<State, Action> Node_ptr;
	std::pair<Result_type, float> search(const Node_ptr& node_ptr,
			const State& goal,
			const float& f_limit,
			const float& max_cost) const;
};

template <typename State,
	typename Action,
	typename Generator,
	typename Heuristic,
	template <typename, typename> class Result_policy>
typename IDA_star_search<State, Action, Generator, Heuristic, Result_policy>::Result_type
IDA_star_search<State, Action, Generator, Heuristic, Result_policy>::operator()(State start,
		State goal,
		float max_cost) const
{
	auto root_ptr = std::make_unique<Node>(this->heuristic_(start, goal), 0, start, Action(), nullptr);
	Result_type result = this->iteration_cutoff();
	float f_limit = root_ptr->f_cost;
	while (result.second == Result::iteration_cutoff)
	{
		auto src_result = search(root_ptr, goal, f_limit, max_cost);
		result = std::move(src_result.first);
		if (result.second != Result::iteration_cutoff)
		{
			return result;
		}
		f_limit = src_result.second;
	}
	return result;
}

template <typename State,
		typename Action,
		typename Generator,
		typename Heuristic,
		template <typename, typename> class Result_policy>
std::pair<typename IDA_star_search<State, Action, Generator, Heuristic, Result_policy>::Result_type, float>
IDA_star_search<State, Action, Generator, Heuristic, Result_policy>::search(const Node_ptr& node_ptr,
		const State& goal,
		const float& f_limit,
		const float& max_cost) const
{
	bool cutoff_occurred = false;
	if (node_ptr->f_cost > f_limit)
	{
		return {this->iteration_cutoff(), node_ptr->f_cost};
	}
	if (node_ptr->g_cost-1 > max_cost)
	{
		return {this->cutoff(), f_limit};
	}
	if (node_ptr->state == goal)
	{
		return {std::make_pair(std::move(Result_policy<State, Action>::make_path(*node_ptr)), Result::success),
			f_limit};
	}

	float min = std::numeric_limits<float>::max();
	for (const auto& successor_ptr : node_ptr->successors(this->generator_, this->heuristic_, goal))
	{
		auto result = search(successor_ptr, goal, f_limit, max_cost);
		if (result.second < min)
		{
			min = result.second;
		}
		if (result.first.second == Result::iteration_cutoff)
		{
			cutoff_occurred = true;
		}
		else if (result.first.second != Result::failure || result.first.second != Result::cutoff)
		{
			return result;
		}
	}
	if (cutoff_occurred)
	{
		return {this->iteration_cutoff(), min};
	}
	return {this->failure(), min};
}


/**
 * @brief IEA* tries to improve over IDA* by relaxing its memory constraints
 */
template <typename State,
		typename Action,
		typename Generator,
		typename Heuristic,
		template <typename, typename> class Result_policy = Full_result>
class IEA_star_search : public IDA_star_search<State, Action, Generator, Heuristic, Result_policy>
{
	typedef IDA_star_search<State, Action, Generator, Heuristic, Result_policy> Base;
public:
	using typename Base::State_type;
	using typename Base::Result_type;
	using typename Base::Result;
	using Base::Base;
	Result_type operator()(State start, State goal, float max_cost = std::numeric_limits<float>::max()) const;
private:
	typedef A_star_node<State, Action> Node;
	typedef IEA_star_node_ptr<State, Action> Node_ptr;
	using Node_set = std::unordered_set<Node_ptr>;
	std::pair<Result_type, float> f_limited_search(const Node_ptr& node_ptr,
			std::vector<Node_ptr>& successors,
			const Node_set& explored,
			const State& goal,
			const float& f_limit,
			float& new_f_limit,
			const float& max_cost) const;
	std::vector<Node_ptr> expand_frontier(Node_ptr node_ptr,
			std::vector<Node_ptr>& successors,
			const Node_set& explored,
			float f_limit) const;
};


template <typename State, typename Action>
inline bool operator>(const IEA_star_node_ptr<State, Action>& lhs,
		const IEA_star_node_ptr<State, Action>& rhs) noexcept
{
	if (lhs->f_cost != rhs->f_cost)
	{
		return lhs->f_cost > rhs->f_cost;
	}
	return lhs->g_cost < rhs->g_cost;
}

template <typename State, typename Action>
inline bool operator==(const IEA_star_node_ptr<State, Action>& lhs,
		const IEA_star_node_ptr<State, Action>& rhs) noexcept
{
	return *lhs == *rhs;
}


template <typename State,
		typename Action,
		typename Generator,
		typename Heuristic,
		template <typename, typename> class Result_policy>
typename IEA_star_search<State, Action, Generator, Heuristic, Result_policy>::Result_type
IEA_star_search<State, Action, Generator, Heuristic, Result_policy>::operator()(State start,
		State goal,
		float max_cost) const
{
	using Frontier = std::priority_queue<Node_ptr,
			std::vector<Node_ptr>,
			std::greater<Node_ptr>>;

	Frontier frontier;
	Node_set explored;

	auto root_ptr = std::make_shared<Node>(this->heuristic_(start, goal), 0, start, Action(), nullptr);
	Result_type result = this->iteration_cutoff();
	float f_limit = root_ptr->f_cost;
	explored.insert(root_ptr);
	frontier.push(std::move(root_ptr));
	while (result.second == Result::iteration_cutoff)
	{
		Frontier new_frontier;
		float new_f_limit = f_limit;
		while (!frontier.empty())
		{
			const auto best = std::move(const_cast<typename Frontier::value_type&>(frontier.top()));
			frontier.pop();
			std::vector<Node_ptr> successors;
			auto src_result = f_limited_search(best, successors, explored, goal, f_limit, new_f_limit, max_cost);
			result = std::move(src_result.first);
			if (result.second != Result::iteration_cutoff)
			{
				return result;
			}
			if ((src_result.second < new_f_limit || new_f_limit == f_limit) && src_result.second > f_limit)
			{
				new_f_limit = src_result.second;
			}
			const auto expansion = expand_frontier(std::move(best), successors, explored, f_limit);
			for (const auto& node_ptr : expansion)
			{
				explored.insert(node_ptr);
				new_frontier.push(std::move(node_ptr));
			}
		}
		frontier = std::move(new_frontier);
		f_limit = new_f_limit;
	}
	return result;
}

template <typename State,
		typename Action,
		typename Generator,
		typename Heuristic,
		template <typename, typename> class Result_policy>
std::pair<typename IEA_star_search<State, Action, Generator, Heuristic, Result_policy>::Result_type, float>
IEA_star_search<State, Action, Generator, Heuristic, Result_policy>::f_limited_search(const Node_ptr& node_ptr,
		std::vector<Node_ptr>& successors,
		const Node_set& explored,
		const State& goal,
		const float& f_limit,
		float& new_f_limit,
		const float& max_cost) const
{
	bool cutoff_occurred = false;
	if (node_ptr->f_cost > f_limit)
	{
		return {this->iteration_cutoff(), node_ptr->f_cost};
	}
	if (node_ptr->g_cost-1 > max_cost)
	{
		return {this->cutoff(), f_limit};
	}
	if (node_ptr->state == goal)
	{
		return {std::make_pair(std::move(Result_policy<State, Action>::make_path(*node_ptr)), Result::success),
			f_limit};
	}
	for (auto&& e : node_ptr->successors(this->generator_, this->heuristic_, goal))
	{
		successors.emplace_back(std::move(e));
	}
	float min = std::numeric_limits<float>::max();
	for (const auto& successor_ptr : successors)
	{
		std::vector<Node_ptr> successors;
		auto result = f_limited_search(successor_ptr, successors, explored, goal, f_limit, new_f_limit, max_cost);
		if (result.second < min)
		{
			min = result.second;
		}
		if (result.first.second == Result::iteration_cutoff)
		{
			cutoff_occurred = true;
		}
		else if (result.first.second != Result::failure || result.first.second != Result::cutoff)
		{
			return result;
		}
	}
	if (cutoff_occurred)
	{
		return {this->iteration_cutoff(), min};
	}
	return {this->failure(), min};
}

template <typename State,
		typename Action,
		typename Generator,
		typename Heuristic,
		template <typename, typename> class Result_policy>
std::vector<typename IEA_star_search<State, Action, Generator, Heuristic, Result_policy>::Node_ptr>
IEA_star_search<State, Action, Generator, Heuristic, Result_policy>::expand_frontier(Node_ptr node_ptr,
		std::vector<Node_ptr>& successors,
		const Node_set& explored,
		float f_limit) const
{
	std::vector<Node_ptr> nodes;
	bool successors_explored = true;
	for (auto it = successors.cbegin(); it != successors.cend(); ++it)
	{
		if (explored.find(*it) == explored.cend())
		{
			if ((*it)->f_cost <= f_limit)
			{
				nodes.push_back(std::move(*it));
			}
			else
			{
				successors_explored = false;
			}
		}
	}
	if (!successors_explored)
	{
		nodes.push_back(std::move(node_ptr));
	}
	return nodes;
}

#endif
