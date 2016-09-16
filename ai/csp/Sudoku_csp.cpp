#include <vector>
#include <memory>
#include <cstddef>

#include <unordered_map>
#include "Sudoku_csp.hpp"

using namespace detail;

namespace
{
	std::vector<Sudoku_board::value_type> get_domain(Pos pos, const Sudoku_values&);
}

bool Sudoku_constraint::hold(const Sudoku_board::value_type& value,
		const Variable<Pos, Sudoku_board::value_type>& variable,
		const Assignment_dispatcher<Pos, Sudoku_board::value_type>& assignment) const
{
	const auto other = (variable.id() == first_) ? second_ : first_;
	return assignment.get_value(other) != value;
}

bool Sudoku_constraint::hold(const Sudoku_board::value_type& value_1, const Sudoku_board::value_type& value_2) const
{
	return value_1 != value_2;
}

Sudoku_csp create_csp(const Sudoku_values& starting_values)
{
	// Initialize the constraints
	Sudoku_constraints constraints;
	std::unordered_map<Pos, Sudoku_constraints_ref> constraints_map;
	for (const auto& block : get_all_blocks_pos())
	{
		constraints_from_block(block, std::back_inserter(constraints), constraints_map);
	}

	// Initialize the variables with full domain
	Sudoku_variables variables;
	variables.reserve(Sudoku_board::size * Sudoku_board::size);
	for (Sudoku_board::value_type i = 0; i < Sudoku_board::size; ++i)
	{
		for (Sudoku_board::value_type j = 0; j < Sudoku_board::size; ++j)
		{
			Pos pos{i, j};
			variables.emplace_back(pos, get_domain(pos, starting_values), constraints_map.at(pos));
		}
	}

	return Sudoku_csp(std::move(variables), std::move(constraints));
}

namespace
{
	std::vector<Sudoku_board::value_type> get_domain(const Pos pos, const Sudoku_values& starting_values)
	{
		auto it = starting_values.find(pos);
		if (it != starting_values.cend())
		{
			return {it->second};
		}
		return {1, 2, 3, 4, 5, 6, 7, 8, 9};
	}
}
