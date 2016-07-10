/**
 * Dividing a range in equal parts in respect to the sums of their elements
 */

#include <vector>
#include <iostream>
#include <cstddef>
#include <algorithm>
#include "partition.hpp"

int main()
{
	std::vector<unsigned> v{1, 2, 3, 4, 5, 6, 7, 8, 9};
	auto dividers = algo::partition(v.cbegin(), v.cend(), 3);
	
	for (std::size_t i = 0; i < dividers.size() - 1; ++i)
	{
		std::cout << "partition " << (i+1) << ": ";
		std::copy(dividers[i], dividers[i+1], std::ostream_iterator<typename decltype(v)::value_type>(std::cout, " "));
		std::cout << "\n";
	}
}
