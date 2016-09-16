#include "Sudoku_board.hpp"
#include <iostream>

namespace
{
	Pos make_pos(unsigned x, unsigned y)
	{
		return Pos{static_cast<Sudoku_board::value_type>(x), static_cast<Sudoku_board::value_type>(y)};
	}
}

bool Sudoku_board::solved() const
{
	for (const auto& block : get_all_blocks_pos())
	{
        if (!check_block(block.cbegin(), block.cend(), *this))
		{
        	return false;
		}
	}
    return true;
}

std::vector<std::array<Pos, Sudoku_board::size>> get_all_blocks_pos()
{
	typedef std::array<Pos, Sudoku_board::size> Block;
	std::vector<Block> ret;

	for (unsigned i = 0; i < Sudoku_board::size; ++i)
	{
		Block block_h;
		Block block_v;
		for (unsigned j = 0; j < Sudoku_board::size; ++j)
		{
			block_h[j] = make_pos(i, j);
			block_v[j] = make_pos(j, i);
		}
		ret.push_back(std::move(block_h));
		ret.push_back(std::move(block_v));
	}

	for (unsigned i = 0; i < Sudoku_board::size; ++i)
	{
        Block block;
        auto j = (i / 3) * 3;
        auto k = (i % 3) * 3;
        block[0] = make_pos(j, k);
        block[1] = make_pos(j, k + 1);
        block[2] = make_pos(j, k + 2);
        block[3] = make_pos(j + 1, k);
        block[4] = make_pos(j + 1, k + 1);
        block[5] = make_pos(j + 1, k + 2);
        block[6] = make_pos(j + 2, k);
        block[7] = make_pos(j + 2, k + 1);
        block[8] = make_pos(j + 2, k + 2);
        ret.push_back(std::move(block));
	}

	return ret;
}

std::ostream& operator<<(std::ostream& os, const Sudoku_board& board)
{
	for (unsigned i = 0; i < Sudoku_board::size; ++i)
	{
		for (unsigned j = 0; j < Sudoku_board::size; ++j)
		{
			os << static_cast<unsigned>(board[i][j]) << " ";
		}
		os << "\n";
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, Pos pos)
{
	os << static_cast<int>(pos.x) << ", " << static_cast<int>(pos.y);
	return os;
}

bool operator<(Pos lhs, Pos rhs) noexcept
{
    if (lhs.x < rhs.x)
    {
        return true;
    }
    else if (lhs.x > rhs.x)
    {
        return false;
    }
    else
    {
        return lhs.y < rhs.y;
    }
}
