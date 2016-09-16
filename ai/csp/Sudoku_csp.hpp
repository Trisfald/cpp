#ifndef AI_CSP_SUDOKU_CSP_HPP_
#define AI_CSP_SUDOKU_CSP_HPP_

#include "Sudoku_board.hpp"
#include "Csp.hpp"
#include <iterator>
#include <unordered_map>
#include <functional>
#include <utility>

typedef Csp<Pos, Sudoku_board::value_type> Sudoku_csp;
typedef Constraints<Pos, Sudoku_board::value_type> Sudoku_constraints;
typedef Constraints_ref<Pos, Sudoku_board::value_type> Sudoku_constraints_ref;
typedef Variables<Pos, Sudoku_board::value_type> Sudoku_variables;
typedef std::unordered_map<Pos, Sudoku_board::value_type> Sudoku_values;

namespace detail
{
	typedef std::vector<std::pair<Pos, Sudoku_board::value_type>> Values_vector;
}

class Sudoku_constraint : public Binary_constraint<Pos, Sudoku_board::value_type>
{
public:
	Sudoku_constraint(Pos first, Pos second) : first_(first), second_(second) {}
    bool hold(const Sudoku_board::value_type&,
    		const Variable<Pos, Sudoku_board::value_type>&,
			const Assignment_dispatcher<Pos, Sudoku_board::value_type>&) const override;
	bool hold(const Sudoku_board::value_type& value_1, const Sudoku_board::value_type& value_2) const override;
	const Pos& variable_1_id() const { return first_; }
	const Pos& variable_2_id() const { return second_; }
private:
    Pos first_;
    Pos second_;
};

Sudoku_csp create_csp(const Sudoku_values& starting_values = Sudoku_values());

template <typename Assignment>
Sudoku_board make_board(const Assignment& assignment)
{
	Sudoku_board board;
	detail::Values_vector values;
	for (const auto& i : assignment)
	{
	    values.push_back({i.first, i.second.value()});
	}
	std::sort(values.begin(), values.end());
	for (unsigned i = 0; i < Sudoku_board::size * Sudoku_board::size; ++i)
	{
	    board[i / Sudoku_board::size][i % Sudoku_board::size] = values[i].second;
	}
	return board;
}

template <typename C, typename Bk_inserter, typename M>
void constraints_from_block(const C& squares, Bk_inserter bk_inserter, M& constraints_map)
{
	auto size = squares.size();
	for (const auto square : squares)
	{
		std::size_t i = 1;
		for (std::size_t j = i; j < size; ++j)
		{
			if (square != squares[j])
			{
				auto ptr = std::make_unique<Sudoku_constraint>(square, squares[j]);
				typedef typename M::mapped_type::value_type constraint_type;
				constraints_map[square].push_back(static_cast<constraint_type>(std::cref(*ptr)));
				constraints_map[squares[j]].push_back(static_cast<constraint_type>(std::cref(*ptr)));
				bk_inserter = std::move(ptr);
			}
		}
		++i;
	}
}

#endif
