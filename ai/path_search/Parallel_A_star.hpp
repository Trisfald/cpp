#ifndef AI_SEARCHING_PARALLEL_A_STAR_HPP_
#define AI_SEARCHING_PARALLEL_A_STAR_HPP_

#include "A_star.hpp"
#include <thread>
#include <future>
#include <mutex>
#include <atomic>
#include <tuple>

namespace detail
{

	struct Lock_data;

}

/**
 * @brief Bidirectional A* search on two separate threads. It loses A* optimality
 */
template <typename State,
		typename Action,
		typename Generator,
		typename Heuristic,
		template <typename, typename> class Result_policy = Full_result>
class BA_star_search : private A_star_search<State, Action, Generator, Heuristic, Result_policy>
{
	typedef A_star_search<State, Action, Generator, Heuristic, Result_policy> Base;
	struct Frontier_data;
public:
	using typename Base::State_type;
	using typename Base::Result_type;
	using typename Base::Result;
	using Base::Base;
	Result_type operator()(State start,
			State goal,
			bool improved_accuracy = true,
			float max_cost = std::numeric_limits<float>::max()) const;
private:
	typedef A_star_node<State, Action> Node;
	typedef A_star_node_ptr<State, Action> Node_ptr;
	using Frontier = std::priority_queue<Node_ptr,
			std::vector<Node_ptr>,
			std::greater<Node_ptr>>;
	using Frontier_set = std::unordered_set<Node*,
			std::hash<Node*>,
			detail::A_star_node_ptr_equality<State, Action>>;
	using Node_set = std::unordered_set<Node_ptr>;
	enum class Partial_result
	{
		failure, cutoff, success, iteration_cutoff, connect
	};
	using Search_result = std::tuple<Node_ptr, Node*, Partial_result>;
	Search_result search(const State& start,
			State& goal,
			float max_cost,
			Frontier_data,
			detail::Lock_data) const;
	void find_best_connect(Node*&, Node*&, const Frontier_set&, const Frontier_set&) const;
};

template <typename State,
		typename Action,
		typename Generator,
		typename Heuristic,
		template <typename, typename> class Result_policy>
struct BA_star_search<State, Action, Generator, Heuristic, Result_policy>::Frontier_data
{
	Frontier& self_frontier;
	Frontier_set& self_frontier_set;
	Node_set& explored;
	const Frontier_set& other_frontier_set;
};

namespace detail
{

	struct Lock_data
	{
		std::mutex& self_m;
		std::mutex& other_m;
		std::atomic<bool>& done_flag;
	};

	template <typename Node, typename Frontier, typename Frontier_set, typename Mutex>
	inline void ba_star_add_frontier(Node&& node,
			Frontier& frontier,
			Frontier_set& frontier_set,
			Mutex& m)
	{
		{
			std::lock_guard<Mutex> lk(m);
			frontier_set.insert(node.get());
		}
		frontier.push(std::move(node));
	}

	template <typename Frontier, typename Frontier_set, typename Mutex>
	inline typename Frontier::value_type ba_star_pop_frontier(Frontier& frontier,
			Frontier_set& frontier_set,
			Mutex& m)
	{
		auto node_ptr = std::move(const_cast<typename Frontier::value_type&>(frontier.top()));
		frontier.pop();
		{
			std::lock_guard<Mutex> lk(m);
			frontier_set.erase(node_ptr.get());
		}
		return node_ptr;
	}

}

template <typename State,
		typename Action,
		typename Generator,
		typename Heuristic,
		template <typename, typename> class Result_policy>
typename BA_star_search<State, Action, Generator, Heuristic, Result_policy>::Result_type
BA_star_search<State, Action, Generator, Heuristic, Result_policy>::operator()(State start,
		State goal,
		bool improved_accuracy,
		float max_cost) const
{
	Frontier frontier_1, frontier_2;
	Frontier_set frontier_set_1, frontier_set_2;
	Node_set explored_1, explored_2;
	std::mutex m_1, m_2;
	std::atomic<bool> done{false};

	auto bw_future = std::async(std::launch::async,
			&BA_star_search<State, Action, Generator, Heuristic, Result_policy>::search,
			this,
			std::ref(goal),
			std::ref(start),
			max_cost,
			Frontier_data{std::ref(frontier_2),
					std::ref(frontier_set_2),
					std::ref(explored_2),
					std::ref(frontier_set_1)},
			detail::Lock_data{std::ref(m_2),	std::ref(m_1), std::ref(done)});
	auto result_fw = search(start,
			goal,
			max_cost,
			Frontier_data{frontier_1, frontier_set_1, explored_1, frontier_set_2},
			detail::Lock_data{m_1, m_2, done});
	auto result_bw = bw_future.get();
	if (std::get<2>(result_fw) == Partial_result::success)
	{
		return std::make_pair(std::move(Result_policy<State, Action>::make_path(*std::get<0>(result_fw))),
				Result::success);
	}
	if (std::get<2>(result_bw) == Partial_result::success)
	{
		return std::make_pair(std::move(Result_policy<State, Action>::make_path(static_cast<Node*>(nullptr),
				std::get<0>(result_bw).get())), Result::success);
	}
	if (std::get<2>(result_fw) == Partial_result::connect)
	{
		auto connect_fw = std::get<0>(result_fw).get();
		auto connect_bw = std::get<1>(result_fw);
		if (improved_accuracy)
		{
			find_best_connect(connect_fw, connect_bw, frontier_set_1, frontier_set_2);
		}
		return std::make_pair(std::move(Result_policy<State, Action>::make_path(connect_fw, connect_bw)),
				Result::success);
	}
	if (std::get<2>(result_bw) == Partial_result::connect)
	{
		auto connect_bw = std::get<0>(result_bw).get();
		auto connect_fw = std::get<1>(result_bw);
		if (improved_accuracy)
		{
			find_best_connect(connect_fw, connect_bw, frontier_set_1, frontier_set_2);
		}
		return std::make_pair(std::move(Result_policy<State, Action>::make_path(connect_fw, connect_bw)),
				Result::success);
	}	
	if (std::get<2>(result_fw) == Partial_result::failure || std::get<2>(result_bw) == Partial_result::failure)
	{
		return this->failure();
	}
	return this->cutoff();
}

template <typename State,
		typename Action,
		typename Generator,
		typename Heuristic,
		template <typename, typename> class Result_policy>
typename BA_star_search<State, Action, Generator, Heuristic, Result_policy>::Search_result
BA_star_search<State, Action, Generator, Heuristic, Result_policy>::search(const State& start,
		State& goal,
		float max_cost,
		Frontier_data ftr_data,
		detail::Lock_data lk_data) const
{
	bool cutoff_occurred = false;
	auto root_ptr = std::make_unique<Node>(this->heuristic_(start, goal), 0, start, Action(), nullptr);
	detail::ba_star_add_frontier(std::move(root_ptr),
			ftr_data.self_frontier,
			ftr_data.self_frontier_set,
			lk_data.self_m);
	while (!lk_data.done_flag.load(std::memory_order_relaxed))
	{
		if (ftr_data.self_frontier.empty())
		{
			return cutoff_occurred ?
					std::make_tuple(nullptr, nullptr, Partial_result::iteration_cutoff) :
					std::make_tuple(nullptr, nullptr, Partial_result::failure);
		}

		auto node_ptr = detail::ba_star_pop_frontier(ftr_data.self_frontier,
				ftr_data.self_frontier_set,
				lk_data.self_m);
		if (node_ptr->state == goal)
		{
			lk_data.done_flag.store(true, std::memory_order_relaxed);
			return std::make_tuple(std::move(node_ptr), nullptr, Partial_result::success);
		}

		{
			std::lock_guard<std::mutex> lk(lk_data.other_m);
			auto connect_it = ftr_data.other_frontier_set.find(node_ptr.get());
			if (connect_it != ftr_data.other_frontier_set.end())
			{
				lk_data.done_flag.store(true, std::memory_order_relaxed);
				return std::make_tuple(std::move(node_ptr), *connect_it, Partial_result::connect);
			}
		}

		auto insert_pair = ftr_data.explored.insert(std::move(node_ptr));
		if ((*insert_pair.first)->g_cost > max_cost)
		{
			cutoff_occurred = true;
			continue;
		}
		for (auto& successor_ptr : (*insert_pair.first)->successors(this->generator_, this->heuristic_, goal))
		{
			auto frontier_successor_it = ftr_data.self_frontier_set.find(successor_ptr.get());
			if (frontier_successor_it == ftr_data.self_frontier_set.end())
			{
				if (ftr_data.explored.find(successor_ptr) == ftr_data.explored.end())
				{
					detail::ba_star_add_frontier(std::move(successor_ptr),
							ftr_data.self_frontier,
							ftr_data.self_frontier_set,
							lk_data.self_m);
				}
			}
			else if ((*frontier_successor_it)->f_cost > successor_ptr->f_cost)
			{
				*(*frontier_successor_it) = *successor_ptr.get();
			}
		}
	}
	return cutoff_occurred ?
			std::make_tuple(nullptr, nullptr, Partial_result::iteration_cutoff) :
			std::make_tuple(nullptr, nullptr, Partial_result::failure);
}

template <typename State,
		typename Action,
		typename Generator,
		typename Heuristic,
		template <typename, typename> class Result_policy>
void BA_star_search<State, Action, Generator, Heuristic, Result_policy>::find_best_connect(Node*& connect_fw,
		Node*& connect_bw,
		const Frontier_set& frontier_set_1,
		const Frontier_set& frontier_set_2) const
{
	float cost = connect_fw->g_cost + connect_bw->g_cost;
	const bool first_set_smaller = frontier_set_1.size() <= frontier_set_2.size();
	const auto& frontier_src = first_set_smaller ? frontier_set_1 : frontier_set_2;
	const auto& frontier_dst = first_set_smaller ? frontier_set_2 : frontier_set_1;
	for (const auto ptr_1 : frontier_src)
	{
		const auto it = frontier_dst.find(ptr_1);
		if (it != frontier_dst.cend())
		{
			float new_cost = ptr_1->g_cost + (*it)->g_cost;
			if (new_cost < cost)
			{
				cost = new_cost;
				connect_fw = first_set_smaller ? ptr_1 : *it;
				connect_bw = first_set_smaller ? *it : ptr_1;
			}
		}
	}
}

#endif
