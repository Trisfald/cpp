#ifndef CONCURRENCY_BENCHMARK_HPP_
#define CONCURRENCY_BENCHMARK_HPP_

#include <iostream>
#include <utility>
#include <chrono>

template <typename Func,
		typename... Args,
		typename std::enable_if_t<!std::is_void<std::result_of_t<Func(Args...)>>::value>* = nullptr>
decltype(auto) benchmark(Func f, Args&&... args)
{
	auto start = std::chrono::steady_clock::now();
	decltype(auto) result = f(std::forward<Args>(args)...);
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
	std::cout << "Time: " << duration.count() << " ms\n";
	return result;
}

template <typename Func,
		typename... Args,
		typename std::enable_if_t<std::is_void<std::result_of_t<Func(Args...)>>::value>* = nullptr>
void benchmark(Func f, Args&&... args)
{
	auto start = std::chrono::steady_clock::now();
	f(std::forward<Args>(args)...);
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
	std::cout << "Time: " << duration.count() << " ms\n";
}

#endif
