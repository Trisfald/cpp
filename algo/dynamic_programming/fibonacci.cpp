/**
 * Dynamic programming techniques applied to a very simple problem: compute Fibonacci numbers
 */

#include <stdexcept>
#include <cstddef>
#include <algorithm>
#include <limits>
#include <array>
#include <iostream>

template <typename T> T f(T);
template <typename T> T clever_f(T);
template <typename T> T no_recursion_f(T);
template <typename T> T slim_f(T);

// Must be defined in order to be used at compile time
template <typename T>
constexpr T compile_time_f(T n)
{
	T back_2 = 0;
	T back_1 = 1;
	T result = 0;
	for (std::size_t i = 2; i <= n; ++i)
	{
		result = back_1 + back_2;
		back_2 = back_1;
		back_1 = result;
	}
	return result;
}


int main()
{
	constexpr unsigned n = 20;
	
	std::cout << f(n) << std::endl;
	std::cout << clever_f(n) << std::endl;
	std::cout << no_recursion_f(n) << std::endl;
	std::cout << slim_f(n) << std::endl;
	// Force compile time evaluation
	constexpr auto result = compile_time_f(n);
	std::cout << result << std::endl;
}

// Classic implementation, slow
template <typename T>
T f(T n)
{
	if (n == 0)
	{
		return 0;
	}
	if (n == 1)
	{
		return 1;
	}
	return f(n-1) + f(n-2);
}

template <typename T, typename A>
T clever_f_call(T n, A& cache)
{
	if (cache[n] == std::numeric_limits<T>::max())
	{
		cache[n] = clever_f_call(n-1, cache) + clever_f_call(n-2, cache);
	}
	return cache[n];
}

// Avoid recomputing whole branches by storing results in a cache
template <typename T>
T clever_f(T n)
{
	constexpr static std::size_t max = 10000;
	if (n > max)
	{
		throw std::logic_error("n is too big");
	}
	std::array<T, max> cache;
	std::fill(cache.begin(), cache.end(), std::numeric_limits<T>::max());
	cache[0] = 0;
	cache[1] = 1;
	return clever_f_call(n, cache);
}

// Avoid recursion by directly filling the array of results
template <typename T>
T no_recursion_f(T n)
{
	constexpr static std::size_t max = 10000;
	if (n > max)
	{
		throw std::logic_error("n is too big");
	}
	std::array<T, max> cache;
	cache[0] = 0;
	cache[1] = 1;
	for (std::size_t i = 2; i <= max; ++i)
	{
		cache[i] = cache[i-1] + cache[i-2];
	}
	return cache[n];
}

// Minimize memory consumption by keeping alive only the last computation
template <typename T>
T slim_f(T n)
{
	T back_2 = 0;
	T back_1 = 1;
	T result = 0;
	for (std::size_t i = 2; i <= n; ++i)
	{
		result = back_1 + back_2;
		back_2 = back_1;
		back_1 = result;
	}
	return result;
}
