#include <vector>
#include <iostream>
#include <algorithm>
#include "Parallel_algorithm.hpp"
#include "Simple_benchmark.hpp"

#define WRAP(f) \
		[] (auto&&... args) -> decltype(auto) \
			{ return f(std::forward<decltype(args)>(args)...); }

void foo(int) {};

int main()
{
	std::vector<int> v;
	for (unsigned i = 0; i < 50000000; ++i)
	{
		v.push_back(i % 1000 == 0 ? 0 : i);
	}

	std::cout << "Standard count" << "\n";
	std::cout << "0 appears " << benchmark(WRAP(std::count),
			v.cbegin(),
			v.cend(),
			0) << " times" << std::endl;

	std::cout << "\n" << "Parallel count" << "\n";
	std::cout << "0 appears " << benchmark(WRAP(parallel_count),
			v.cbegin(),
			v.cend(),
			0) << " times" << std::endl;

	std::cout << "\n" <<"Standard count_if" << "\n";
	std::cout << "Multiples of 4 appear " << benchmark(WRAP(std::count_if),
			v.cbegin(),
			v.cend(),
			[](auto& i){return i % 4 == 0;})-1 << " times" << std::endl;

	std::cout << "\n" << "Parallel count_if" << "\n";
	std::cout << "Multiples of 4 appear " << benchmark(WRAP(parallel_count_if),
			v.cbegin(),
			v.cend(),
			[](auto& i){return i % 4 == 0;})-1 << " times" << std::endl;

	std::cout << "\n" << "Standard replace" << "\n";
	benchmark(WRAP(std::replace), v.begin(), v.end(), 0, -1);

	std::cout << "\n" << "Parallel replace" << "\n";
	benchmark(WRAP(parallel_replace), v.begin(), v.end(), -1, 0);
}
