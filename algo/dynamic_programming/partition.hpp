#ifndef ALGO_DYNAMIC_PROGRAMMING_PARTITION_HPP_
#define ALGO_DYNAMIC_PROGRAMMING_PARTITION_HPP_

#include <cstddef>
#include <iterator>
#include <vector>

namespace algo
{

	template <typename Iterator, typename Matrix>
	void extract_partitions(Iterator begin,
			Iterator end,
			const Matrix& dividers,
			std::size_t n,
			std::size_t k,
			std::vector<Iterator>& results)
	{
		if (k == 0)
		{
			results.push_back(begin);
		}
		else
		{
			extract_partitions(begin, end, dividers, dividers[n][k], k - 1, results);
			auto it = begin;
			std::advance(it, n);
			results.push_back(it);
		}
	}

	template <typename Iterator>
	std::vector<Iterator> partition(Iterator begin, Iterator end, std::size_t k)
	{
		typedef typename Iterator::value_type value_type;
		typedef std::vector<std::vector<value_type>> Matrix;
		
		std::size_t n = static_cast<std::size_t>(std::distance(begin, end));
		Matrix values(n + 1, std::vector<value_type>(k + 1, std::numeric_limits<value_type>::max()));
		std::vector<value_type> sums;
		sums.reserve(n + 1);
		
		sums.push_back(0);
		{
			auto it = begin;
			for (std::size_t i = 1; i <= n; ++i) 
			{
				sums.push_back(sums[i - 1] + *it);
				values[i][1] = sums[i];
				++it;
			}
		}
		for (std::size_t j = 1; j <= k; ++j)
		{
			values[1][j] = *begin;
		}
		
		Matrix dividers(n + 1, std::vector<value_type>(k + 1, std::numeric_limits<value_type>::max()));
		
		for (std::size_t i = 2; i <= n; ++i)
		{
			for (std::size_t j = 2; j <= k; ++j) 
			{
				for (std::size_t x = 1; x <= i-1; ++x)
				{
					auto cost = std::max(values[x][j-1], sums[i]-sums[x]);
					if (values[i][j] > cost)
					{
						values[i][j] = cost;
						dividers[i][j] = x;
					}
				}			
			}
		}
		std::vector<Iterator> results;
		extract_partitions(begin, end, dividers, n, k, results);
		return results;
	}
	
}

#endif
