#ifndef CONCURRENCY_PARALLEL_ALGORITHM_HPP_
#define CONCURRENCY_PARALLEL_ALGORITHM_HPP_

#include <thread>
#include <algorithm>
#include <iterator>
#include <future>
#include <vector>
#include <type_traits>
#include "Thread_joiner.hpp"

namespace detail
{
	template <typename S, typename N>
	inline std::pair<S, N> get_threads_and_block_size(S size, N num_threads_hint, N min_items_per_thread)
	{
		const N max_threads = (size + min_items_per_thread - 1) / min_items_per_thread;
		const N num_threads = std::min(num_threads_hint != 0 ? num_threads_hint : 2, max_threads);
		const S block_size = size / num_threads;
		return std::make_pair(num_threads, block_size);
	}

	template <typename It,
		typename Func,
		typename... Args,
		typename std::enable_if_t<!std::is_void<std::result_of_t<Func(It, It, Args...)>>::value>* = nullptr>
	auto block_parallelizer(unsigned num_threads_hint,
			unsigned min_items_per_thread,
			It begin,
			It end,
			Func func,
			Args&&... args) -> std::result_of_t<Func(It, It, Args...)>
	{
		typedef std::result_of_t<Func(It, It, Args...)> Diff_type;

		const auto size = std::distance(begin, end);
		if (size == 0)
			return Diff_type();

		const auto threads_data = detail::get_threads_and_block_size(size, num_threads_hint, min_items_per_thread);
		const auto num_threads = threads_data.first;
		const auto block_size = threads_data.second;

		std::vector<std::future<Diff_type>> futures(num_threads - 1);
		std::vector<std::thread> threads(num_threads - 1);
		Thread_joiner joiner(threads);
		It block_begin = begin;

		for (unsigned long i = 0; i < (num_threads - 1); ++i)
		{
			It block_end = block_begin;
			std::advance(block_end, block_size);
			std::packaged_task<Diff_type(void)> task([block_begin, block_end, &func, &args...]()
					{
						return func(block_begin, block_end, std::forward<Args>(args)...);
					});
			futures[i] = task.get_future();
			threads[i] = std::thread(std::move(task));
			block_begin = block_end;
		}
		Diff_type result = func(block_begin, end, std::forward<Args>(args)...);
		for (unsigned i = 0; i < (num_threads - 1); ++i)
		{
			result += futures[i].get();
		}
		return result;
	}

	template <typename It,
		typename Func,
		typename... Args,
		typename std::enable_if_t<std::is_void<std::result_of_t<Func(It, It, Args...)>>::value>* = nullptr>
	auto block_parallelizer(unsigned num_threads_hint,
			unsigned min_items_per_thread,
			It begin,
			It end,
			Func func,
			Args&&... args) -> std::result_of_t<Func(It, It, Args...)>
	{
		typedef std::result_of_t<Func(It, It, Args...)> Diff_type;

		const auto size = std::distance(begin, end);
		if (size == 0)
			return Diff_type();

		const auto threads_data = detail::get_threads_and_block_size(size, num_threads_hint, min_items_per_thread);
		const auto num_threads = threads_data.first;
		const auto block_size = threads_data.second;

		std::vector<std::future<Diff_type>> futures(num_threads - 1);
		std::vector<std::thread> threads(num_threads - 1);
		Thread_joiner joiner(threads);
		It block_begin = begin;

		for (unsigned long i = 0; i < (num_threads - 1); ++i)
		{
			It block_end = block_begin;
			std::advance(block_end, block_size);
			std::packaged_task<Diff_type(void)> task([block_begin, block_end, &func, &args...]()
					{
						func(block_begin, block_end, std::forward<Args>(args)...);
					});
			futures[i] = task.get_future();
			threads[i] = std::thread(std::move(task));
			block_begin = block_end;
		}
		func(block_begin, end, std::forward<Args>(args)...);
		for (unsigned i = 0; i < (num_threads - 1); ++i)
		{
			futures[i].get();
		}
	}
}

template <typename It, typename T>
typename std::iterator_traits<It>::difference_type parallel_count(It begin,
		It end,
		const T& value,
		unsigned num_threads_hint = std::thread::hardware_concurrency(),
		unsigned min_items_per_thread = 100)
{
	return detail::block_parallelizer(num_threads_hint, 
		min_items_per_thread, 
		begin, 
		end, 
		[] (auto&&... args) -> decltype(auto) { return std::count(std::forward<decltype(args)>(args)...); }, 
		value);
}

template <typename It, typename Predicate>
typename std::iterator_traits<It>::difference_type parallel_count_if(It begin,
		It end,
		Predicate predicate,
		unsigned num_threads_hint = std::thread::hardware_concurrency(),
		unsigned min_items_per_thread = 100)
{
	return detail::block_parallelizer(num_threads_hint, 
		min_items_per_thread, 
		begin, 
		end, 
		[] (auto&&... args) -> decltype(auto) { return std::count_if(std::forward<decltype(args)>(args)...); }, 
		predicate);
}

template <typename It, typename T>
void parallel_replace(It begin,
		It end,
		const T& old_value,
		const T& new_value,
		unsigned num_threads_hint = std::thread::hardware_concurrency(),
		unsigned min_items_per_thread = 100)
{
	return detail::block_parallelizer(num_threads_hint, 
		min_items_per_thread, 
		begin, 
		end, 
		[] (auto&&... args) -> decltype(auto) { return std::replace(std::forward<decltype(args)>(args)...); },
		old_value, 
		new_value);
}

#endif
