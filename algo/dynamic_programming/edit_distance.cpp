/**
 * Edit distance between strings, done with dynamic programming via bottom-up approach
 */

#include <vector>
#include <string>
#include <tuple>
#include <iostream>
#include <limits>
#include <algorithm>

enum class Action { replace, insert, remove, match, none };
std::ostream& operator<<(std::ostream& os, Action action);

typedef std::pair<float, Action> Item;
using Cost_inrem = float (*) (char);
using Cost_rep = Item (*) (char, char);

float uniform_cost(char) { return 1.0f; }
Item replace_cost(char x, char y) 
{
    return (x == y) ? Item{0.0f, Action::match} : Item{1.0f, Action::replace};
}
std::vector<Action> edit_distance(const std::string& str,
		const std::string& src,
		Cost_rep replace_cost = replace_cost,
		Cost_inrem insert_cost = uniform_cost,
		Cost_inrem remove_cost = uniform_cost);
void init_row(unsigned i,
		std::vector<std::vector<Item>>&,
		const std::string& str,
		const std::string& src,
		Cost_inrem = uniform_cost);
void init_column(unsigned i,
		std::vector<std::vector<Item>>&,
		const std::string& str,
		const std::string& src,
		Cost_inrem = uniform_cost);

int main()
{
	std::string str("kitten");
	std::string src("sitting");
	std::cout << "Transforming [" << str << "] in [" << src << "] - " << "Steps:\n";
	auto ret = edit_distance(str, src);
	for (const auto action : ret)
	{
		std::cout << action << "\n";
	}
}

std::vector<Action> edit_distance(const std::string& str,
		const std::string& src,
		Cost_rep replace_cost,
		Cost_inrem insert_cost,
		Cost_inrem remove_cost)
{
	auto str_len = str.size();
	auto src_len = src.size();

	// Dp table
	std::vector<std::vector<Item>> dp(str_len + 1, std::vector<Item>(src_len + 1,  
            { std::numeric_limits<float>::infinity(), Action::none}));

	// Base cases
    for (unsigned i = 0; i < src_len + 1; ++i)
    {
        init_row(i, dp, str, src, insert_cost);
    }
    for (unsigned i = 0; i < str_len + 1; ++i)
    {
        init_column(i, dp, str, src, remove_cost);
    }

	// Main loop
	for (int i = str_len - 1; i >= 0; --i)
	{
		for (int j = src_len - 1; j >= 0; --j)
		{
            auto replace_result = replace_cost(str[i], src[j]);
			auto replace_score = replace_result.first + dp[i + 1][j + 1].first;
			auto insert_score = insert_cost(src[j]) + dp[i][j + 1].first;
			auto remove_score = remove_cost(str[i]) + dp[i + 1][j].first;            
            
            dp[i][j].first = replace_score;
            dp[i][j].second = replace_result.second;
            
            if (insert_score < dp[i][j].first)
            {
                dp[i][j].first = insert_score;
                dp[i][j].second = Action::insert;
            }
            if (remove_score < dp[i][j].first)
            {
                dp[i][j].first = remove_score;
                dp[i][j].second = Action::remove;
            }
		}
	}

	std::vector<Action> result;
	int i = 0, j = 0;
	do
	{
		result.push_back(dp[i][j].second);
        if (dp[i][j].second == Action::insert)
        {
            ++j;
        }
        else if (dp[i][j].second == Action::remove)
        {
            ++i;
        }
        else if (dp[i][j].second == Action::match || dp[i][j].second == Action::replace)
        {
            ++i;
            ++j;
        }        
	}
	while (dp[i][j].second != Action::none);

	return result;
}

void init_row(unsigned i,
		std::vector<std::vector<Item>>& dp,
		const std::string& str,
		const std::string& src,
		Cost_inrem insert_cost)
{
    dp[str.size()][i].first = std::accumulate(src.cbegin() + i,
    		src.cend(),
			0,
			[&insert_cost] (float cost, auto c) { return cost + insert_cost(c); });
    if (i < src.size())
    {
        dp[str.size()][i].second = Action::insert;
    }
    else
    {
        dp[str.size()][i].second = Action::none;
    }
}

void init_column(unsigned i,
		std::vector<std::vector<Item>>& dp,
		const std::string& str,
		const std::string& src,
		Cost_inrem remove_cost)
{
    dp[i][src.size()].first = std::accumulate(str.cbegin() + i,
    		str.cend(),
			0,
			[&remove_cost] (float cost, auto c) { return cost + remove_cost(c); });
    if (i < str.size())
    {
        dp[i][src.size()].second = Action::remove;
    }
    else
    {
        dp[i][src.size()].second = Action::none;
    }
}

std::ostream& operator<<(std::ostream& os, Action action)
{
	switch (action)
	{
		case Action::replace: return os << "replace";
		case Action::insert: return os << "insert";
		case Action::remove: return os << "remove";
        case Action::match: return os << "match";
		case Action::none: return os << "none";
	}
	return os;
}
