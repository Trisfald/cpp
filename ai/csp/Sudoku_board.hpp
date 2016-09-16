#ifndef AI_CSP_SUDOKU_BOARD_HPP_
#define AI_CSP_SUDOKU_BOARD_HPP_

#include <array>
#include <vector>
#include <set>
#include <cstddef>
#include <iosfwd>

class Sudoku_board
{
public:
    typedef unsigned char value_type;
    decltype(auto) operator[](std::size_t i) const
    {
        return board_[i];
    }
    decltype(auto) operator[](std::size_t i)
    {
        return board_[i];
    }
    bool solved() const;
    constexpr static unsigned size = 9;
    constexpr static value_type empty = 0;
private:
    std::array<std::array<value_type, size>, size> board_ = {};
};

std::ostream& operator<<(std::ostream&, const Sudoku_board&);

struct Pos
{
	Sudoku_board::value_type x;
	Sudoku_board::value_type y;
};

std::ostream& operator<<(std::ostream&, Pos);

bool operator<(Pos lhs, Pos rhs) noexcept;

inline bool operator==(Pos lhs, Pos rhs) noexcept
{
	return lhs.x == rhs.x && lhs.y == rhs.y;
}

inline bool operator!=(Pos lhs, Pos rhs) noexcept
{
	return !(lhs == rhs);
}

namespace std
{
	template <>
	struct hash<Pos>
	{
		std::size_t operator()(Pos pos) const noexcept
		{
			return pos.y + (pos.x << 8);
		}
	};
}

template <typename It>
bool check_block(It begin, It end, const Sudoku_board& board)
{
    std::set<Sudoku_board::value_type> set;
    while (begin != end)
    {
    	auto value = board[begin->x][begin->y];
        if (value == Sudoku_board::empty || !set.insert(value).second)
        {
            return false;
        }
        ++begin;
    }
    return true;
}

std::vector<std::array<Pos, Sudoku_board::size>> get_all_blocks_pos();

#endif
