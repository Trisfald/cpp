#ifndef AI_GAMES_ALPHA_BETA_HPP_
#define AI_GAMES_ALPHA_BETA_HPP_

#include "Alpha_beta_decl.hpp"
#include <chrono>
#include <memory>
#include <limits>

template <typename State, typename Action, typename Generator, typename Interrupt, typename Eval>
class Alfa_beta_search
{
	typedef std::unique_ptr<Action> Action_ptr;
public:
	typedef float num_t;
	Alfa_beta_search(const Generator& generator = Generator(),
			const Interrupt& interrupt = Interrupt(),
			const Eval& eval = Eval())
	: generator_(generator), interrupt_(interrupt), eval_(eval) {}
	Action_ptr operator()(State state);
private:
	num_t max_value(const State&, num_t alpha, num_t beta, unsigned depth);
	num_t min_value(const State&, num_t alpha, num_t beta, unsigned depth);

	Generator generator_;
	Interrupt interrupt_;
	Eval eval_;
};

namespace detail
{

	template <typename T>
	constexpr T min()
	{
		return std::numeric_limits<T>::has_infinity
				? -std::numeric_limits<T>::infinity()
				: std::numeric_limits<T>::lowest();
	}

	template <typename T>
	constexpr T max()
	{
		return std::numeric_limits<T>::has_infinity
				? std::numeric_limits<T>::infinity()
				: std::numeric_limits<T>::max();
	}

}

template <typename State, typename Action, typename Generator, typename Interrupt, typename Eval>
typename Alfa_beta_search<State, Action, Generator, Interrupt, Eval>::Action_ptr
Alfa_beta_search<State, Action, Generator, Interrupt, Eval>::operator()(State state)
{
	auto interrupt = interrupt_(state, 1);
	if (interrupt != Search_interrupt::none)
	{
		return nullptr;
	}
	auto children = generator_(state);
	if (children.empty())
	{
		return nullptr;
	}
	if (children.size() == 1)
	{
		return std::make_unique<Action>(*children.cbegin());
	}
	num_t value = detail::min<num_t>();
	num_t alpha = detail::min<num_t>();
	num_t beta = detail::max<num_t>();
	const Action* best = nullptr;
	for (const auto& child : children)
	{
		auto new_value = min_value(state.successor(child), alpha, beta, 2);
		if (new_value > value)
		{
			value = new_value;
			best = &child;
		}
		if (value >= beta)
		{
			return std::make_unique<Action>(child);
		}
		alpha = std::max(alpha, value);
	}
	return std::make_unique<Action>(*best);
}

template <typename State, typename Action, typename Generator, typename Interrupt, typename Eval>
typename Alfa_beta_search<State, Action, Generator, Interrupt, Eval>::num_t
Alfa_beta_search<State, Action, Generator, Interrupt, Eval>::max_value(const State& state,
		num_t alpha,
		num_t beta,
		unsigned depth)
{
	auto interrupt = interrupt_(state, depth);
	if (interrupt != Search_interrupt::none)
	{
		return eval_(state, interrupt);
	}
	auto children = generator_(state);
	if (children.empty())
	{
		return eval_(state, Search_interrupt::terminal);
	}
	num_t value = detail::min<num_t>();
	for (const auto& child : children)
	{
		value = std::max(value, min_value(state.successor(child), alpha, beta, depth + 1));
		if (value >= beta)
		{
			return value;
		}
		alpha = std::max(alpha, value);
	}
	return value;
}

template <typename State, typename Action, typename Generator, typename Interrupt, typename Eval>
typename Alfa_beta_search<State, Action, Generator, Interrupt, Eval>::num_t
Alfa_beta_search<State, Action, Generator, Interrupt, Eval>::min_value(const State& state,
		num_t alpha,
		num_t beta,
		unsigned depth)
{
	auto interrupt = interrupt_(state, depth);
	if (interrupt != Search_interrupt::none)
	{
		return eval_(state, interrupt);
	}
	auto children = generator_(state);
	if (children.empty())
	{
		return eval_(state, Search_interrupt::terminal);
	}
	num_t value = detail::max<num_t>();
	for (const auto& child : children)
	{
		value = std::min(value, max_value(state.successor(child), alpha, beta, depth + 1));
		if (value <= alpha)
		{
			return value;
		}
		beta = std::min(beta, value);
	}
	return value;
}


#endif
