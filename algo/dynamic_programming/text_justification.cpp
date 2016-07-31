/**
 * Justify text into uniform lines
 */

#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <iterator>
#include <limits>
#include <cmath>

typedef float (*Eval) (std::vector<std::string>::const_iterator, std::vector<std::string>::const_iterator, unsigned);
std::vector<std::string> justify(const std::vector<std::string>& words, unsigned width, Eval);
float eval_fn(std::vector<std::string>::const_iterator begin,
		std::vector<std::string>::const_iterator end,
		unsigned width);

int main()
{
	std::string text("Mauris tincidunt gravida turpis, eu posuere enim molestie a. Pellentesque quis sapien eget ante"
			" viverra iaculis at eget lacus. Nunc tincidunt felis metus, mattis pharetra velit lobortis nec. "
			"Proin viverra volutpat mi, id efficitur justo aliquam vel. Nam at odio eu dui ultricies cursus nec non "
			"ligula. Suspendisse id justo vel ante fermentum aliquet sit amet in nisl. Ut imperdiet ornare massa non "
			"dictum. Ut eu elementum purus. Nunc aliquam enim est, et imperdiet neque pharetra quis. Ut at maximus "
			"augue. In vestibulum risus in nunc laoreet suscipit.");

	std::vector<std::string> words;
    std::stringstream ss(text);
    std::string item;
    while (std::getline(ss, item, ' '))
    {
        words.push_back(item);
    }
    auto result = justify(words, 100, eval_fn);
    std::copy(result.cbegin(), result.cend(), std::ostream_iterator<std::string>(std::cout, "\n"));
}

float eval_fn(std::vector<std::string>::const_iterator begin,
		std::vector<std::string>::const_iterator end,
		unsigned width)
{
	auto words_width = std::accumulate(begin,
			end,
			std::distance(begin, end) - 1,
			[] (unsigned u, const auto& s) { return u + s.size(); });

	int difference = width - words_width;
	return (difference >= 0) ? std::pow(difference, 3) : std::numeric_limits<float>::infinity();
}

std::vector<std::string> justify(const std::vector<std::string>& words, unsigned width, Eval eval_fn)
{
	auto n = words.size();
	std::vector<std::string> result;
	if (n == 0)
	{
		return result;
	}

	// Subproblems result array
	std::vector<float> dp(n, std::numeric_limits<float>::infinity());
	dp[n - 1] = 0; // base case
	// Parent array
	std::vector<unsigned> parents(n, 0);

	for (int i = n - 1; i >= 0; --i)
	{
		for (unsigned j = i + 1; j < n; ++j)
		{
			float score = dp[j] + eval_fn(words.cbegin() + i, words.cbegin() + j, width);
			if (dp[i] > score)
			{
				dp[i] = score;
				parents[i] = j;
			}
		}
	}

	int i = 0;
	do
	{
		auto line = std::accumulate(words.cbegin() + ((i == 0) ? 0 : i + 1),
				words.cbegin() + parents[i] + 1,
				std::string(),
				[] (auto& line, const auto& s) { return line += s + " "; });
		i = parents[i];
		result.push_back(line);
	}
	while (parents[i] != 0);
	return result;
}
