#ifndef AI_SEARCHING_GA_HPP_
#define AI_SEARCHING_GA_HPP_

#include <vector>
#include <utility>
#include <random>
#include <chrono>
#include <cstddef>
#include <future>
#include <functional>
#include <algorithm>
#include "Thread_pool.hpp"

namespace detail
{

	constexpr float default_pop_size = 1000;

}

/**
 * @brief Generic genetic algorithm
 *
 * @tparam T type of the individuals
 * @tparam Generator callable returning a random population
 * @tparam Fitness callable returning the estimated fitness of an individual (the lower the better)
 * @tparam Mutagen callable applying a mutation to an individual
 */
template <typename T, typename Generator, typename Fitness, typename Mutagen>
class GA
{
public:
	GA(const Generator& generator = Generator(),
			const Fitness& fitness = Fitness(),
			const Mutagen& mutagen = Mutagen(),
			float mutation_p = 0.01f,
			float select_factor = 0.05f)
	: generator_(generator),
			fitness_(fitness),
			mutagen_(mutagen),
			random_(std::chrono::system_clock::now().time_since_epoch().count(),
					mutation_p,
					select_factor)
	{}
	T operator()(unsigned max_iterations, unsigned population_size = detail::default_pop_size);
protected:
	typedef std::pair<T, float> I;
	typedef std::vector<I> Population;
	struct Random
	{
		std::mt19937 re;
		std::bernoulli_distribution b_dist;
		std::geometric_distribution<std::size_t> g_dist;
		template <typename Seed>
		Random(const Seed& seed, float b, float g) : re(seed), b_dist(b), g_dist(g) {}
	};
	I reproduce(const I& x, const I& y, Random& random);
	const I& random_selection(const Population& pop, Random& random, const I* const exclude = nullptr);
	inline void mutate(I& i, Random& random);
	Generator generator_;
	Fitness fitness_;
	Mutagen mutagen_;
	Random random_;
};

namespace detail
{

	template <typename I, typename Ptr>
	inline void check_update_best(const I& child, Ptr& best)
	{
		if (best == nullptr)
		{
			best = std::make_unique<I>(child);
		}
		else if (child.second < best->second)
		{
			*best = child;
		}
	}

	template <typename Ptr>
	inline void check_update_best(Ptr&& child, Ptr& best)
	{
		if (best == nullptr || child->second < best->second)
		{
			best = std::move(child);
		}
	}

}

template <typename T, typename Generator, typename Fitness, typename Mutagen>
T GA<T, Generator, Fitness, Mutagen>::operator()(unsigned max_iterations, unsigned population_size)
{
	Population population;
	Population new_population(population_size);
	for (const auto& e : generator_(population_size, random_.re))
	{
		population.push_back(std::make_pair(std::move(e), 0.0f));
	}
	std::unique_ptr<I> best;
	for (unsigned i = 0; i < max_iterations; ++i)
	{		
		std::sort(population.begin(),
				population.end(),
				[](const auto& lhs, const auto& rhs){ return lhs.second < rhs.second; });
		for (std::size_t i = 0; i < population.size(); ++i)
		{
			const auto& x = random_selection(population, random_);
			const auto& y = random_selection(population, random_, &x);
			I child = reproduce(x, y, random_);
			detail::check_update_best(child, best);
			if (child.second == 0)
			{
				return child.first;
			}
			if (random_.b_dist(random_.re))
			{
				mutate(child, random_);
				detail::check_update_best(child, best);
				if (child.second == 0)
				{
					return child.first;
				}
			}
			new_population[i] = std::move(child);
		}
		population.swap(new_population);
	}
	return best->first;
}

template <typename T, typename Generator, typename Fitness, typename Mutagen>
typename GA<T, Generator, Fitness, Mutagen>::I
GA<T, Generator, Fitness, Mutagen>::reproduce(const I& x, const I& y, Random& random)
{
	auto child = std::make_pair(generator_(x.first, y.first, random.re), 0);
	child.second = fitness_(child.first);
	return child;
}

template <typename T, typename Generator, typename Fitness, typename Mutagen>
const typename GA<T, Generator, Fitness, Mutagen>::I&
GA<T, Generator, Fitness, Mutagen>::random_selection(const Population& pop, Random& random, const I* const exclude)
{
	std::size_t index;
	do 
	{
		index = random.g_dist(random.re);
	}
	while (index >= pop.size() || &pop[index] == exclude);
	return pop[index];
}

template <typename T, typename Generator, typename Fitness, typename Mutagen>
void GA<T, Generator, Fitness, Mutagen>::mutate(I& i, Random& random)
{
	mutagen_(i.first, random.re);
	i.second = fitness_(i.first);
}


/**
 * @brief Parallel generic genetic algorithm
 *
 * @tparam T type of the individuals
 * @tparam Generator callable returning a random population
 * @tparam Fitness callable returning the estimated fitness of an individual (the lower the better)
 * @tparam Mutagen callable applying a mutation to an individual
 */
template <typename T, typename Generator, typename Fitness, typename Mutagen>
class PGA : GA<T, Generator, Fitness, Mutagen>
{
	typedef GA<T, Generator, Fitness, Mutagen> Base;
public:
	using Base::GA;
	T operator()(unsigned max_iterations, 
			unsigned population_size = detail::default_pop_size,
			unsigned num_threads_hint = std::thread::hardware_concurrency());
private:
	typedef std::pair<T, float> I;
	typedef std::vector<I> Population;
	typedef typename Population::size_type Pop_size_type;
	using typename Base::Random;
	std::unique_ptr<I> task(const Population& population,
			Population& new_population,
			Pop_size_type first,
			Pop_size_type last,
			Random& random);
	static constexpr unsigned min_items_per_thread = 50;
};

template <typename T, typename Generator, typename Fitness, typename Mutagen>
T PGA<T, Generator, Fitness, Mutagen>::operator()(unsigned max_iterations,
		unsigned population_size,
		unsigned num_threads_hint)
{	
	const unsigned max_threads = (population_size + min_items_per_thread - 1) / min_items_per_thread;
	const unsigned num_threads = std::min(num_threads_hint != 0 ? num_threads_hint : 2, max_threads);
	const unsigned block_size = population_size / num_threads;
	Thread_pool pool_(num_threads - 1);
	std::vector<std::future<std::unique_ptr<I>>> futures(num_threads - 1);
	std::vector<Random> randoms;
	std::uniform_int_distribution<unsigned> d(0, 1000);
	for (unsigned i = 0; i < (num_threads - 1); ++i)
	{
		randoms.emplace_back(d(this->random_.re) * std::chrono::system_clock::now().time_since_epoch().count(),
				this->random_.b_dist.p(),
				this->random_.g_dist.p());
	}
	Population population;
	Population new_population(population_size);
	std::unique_ptr<I> best;
	for (const auto& e : this->generator_(population_size, this->random_.re))
	{
		population.push_back(std::make_pair(std::move(e), 0.0f));
	}

	// Preliminary sort
	std::sort(population.begin(),
			population.end(),
			[](const auto& lhs, const auto& rhs){ return lhs.second < rhs.second; });
	for (unsigned i = 0; i < max_iterations; ++i)
	{
		// Merge
		for (unsigned i = 0; i < (num_threads - 1); ++i)
		{				
			std::inplace_merge(population.begin(), 
					population.begin() + (i + 1) * block_size,
					(i < num_threads - 2) ? (population.begin() + (i + 2) * block_size) : population.end(),
					[](const auto& lhs, const auto& rhs){ return lhs.second < rhs.second; });
		}		
		// Run tasks
		for (unsigned i = 0; i < (num_threads - 1); ++i)
		{
			futures[i] = pool_.submit(std::bind(&PGA<T, Generator, Fitness, Mutagen>::task,
					this,
					std::ref(population),
					std::ref(new_population),
					i * block_size,
					(i + 1) * block_size,
					std::ref(randoms[i])));
		}
		auto this_best = task(population,
				new_population,
				(num_threads - 1) * block_size,
				population_size,
				this->random_);
		// Wait for all
		for (unsigned long i = 0; i < (num_threads - 1); ++i)
		{
			detail::check_update_best(std::move(futures[i].get()), best);
		}		
		detail::check_update_best(std::move(this_best), best);
		if (best->second == 0)
		{
			return best->first;
		}
		population.swap(new_population);
	}
	return best->first;	
}

template <typename T, typename Generator, typename Fitness, typename Mutagen>
std::unique_ptr<typename PGA<T, Generator, Fitness, Mutagen>::I>
PGA<T, Generator, Fitness, Mutagen>::task(const Population& population,
		Population& new_population,
		Pop_size_type first,
		Pop_size_type last,
		Random& random)
{
	std::unique_ptr<I> best;
	for (Pop_size_type i = first; i < last; ++i)
	{
		const auto& x = this->random_selection(population, random);
		const auto& y = this->random_selection(population, random, &x);
		I child = this->reproduce(x, y, random);
		detail::check_update_best(child, best);
		if (random.b_dist(random.re))
		{
			this->mutate(child, random);
			detail::check_update_best(child, best);
		}
		new_population[i] = std::move(child);
	}
	std::sort(new_population.begin() + first,
			new_population.begin() + last,
			[](const auto& lhs, const auto& rhs){ return lhs.second < rhs.second; });
	return best;
}

#endif
