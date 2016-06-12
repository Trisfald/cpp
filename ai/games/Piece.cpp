#include "Piece.hpp"

#include "Chessboard.hpp"
#include <cstdlib>
#include <algorithm>
#include <iostream>

std::ostream& operator<<(std::ostream& os, const Piece& piece)
{
	os << piece.get_symbol();
	return os;
}

bool King::can_move(Pos dst, const Chessboard& board, bool allow_invulnerable) const
{
	auto di = static_cast<int>(dst.first) - static_cast<int>(pos.first);
	auto dj = static_cast<int>(dst.second) - static_cast<int>(pos.second);
	if (std::abs(di) > 1 || std::abs(dj) > 1)
	{
		return false;
	}
	return detail::square_empty_or_enemy(*this, dst, board, allow_invulnerable);
}

std::vector<Move> King::all_moves(const Chessboard& board) const
{
	std::vector<Move> moves;
	for (unsigned char i = (pos.first > 0) ? pos.first - 1 : 0; i < Chessboard::size && i <= pos.first + 1; ++i)
	{
		for (unsigned char j = (pos.second > 0) ? pos.second - 1 : 0; j < Chessboard::size && j <= pos.second + 1; ++j)
		{
			if (detail::square_empty_or_enemy(*this, {i, j}, board))
			{
				moves.emplace_back(Move{pos, {i, j}});
			}
		}
	}
	return moves;
}

bool Queen::can_move(Pos dst, const Chessboard& board, bool allow_invulnerable) const
{
	return detail::check_straight(*this, dst, board, allow_invulnerable)
			|| detail::check_diagonal(*this, dst, board, allow_invulnerable);
}

std::vector<Move> Queen::all_moves(const Chessboard& board) const
{
	std::vector<Move> moves;
	detail::add_diagonals(*this, board, moves);
	detail::add_straights(*this, board, moves);
	return moves;
}

bool Rook::can_move(Pos dst, const Chessboard& board, bool allow_invulnerable) const
{
	return detail::check_straight(*this, dst, board, allow_invulnerable);
}

std::vector<Move> Rook::all_moves(const Chessboard& board) const
{
	std::vector<Move> moves;
	detail::add_straights(*this, board, moves);
	return moves;
}

bool Bishop::can_move(Pos dst, const Chessboard& board, bool allow_invulnerable) const
{
	return detail::check_diagonal(*this, dst, board, allow_invulnerable);
}

std::vector<Move> Bishop::all_moves(const Chessboard& board) const
{
	std::vector<Move> moves;
	detail::add_diagonals(*this, board, moves);
	return moves;
}

bool Knight::can_move(Pos dst, const Chessboard& board, bool allow_invulnerable) const
{
	if (dst.first < pos.first)
	{
		if (dst.second < pos.second)
		{
			if (dst.first == pos.first - 1 && dst.second == pos.second - 2)
			{
				return detail::square_empty_or_enemy(*this, dst, board, allow_invulnerable);
			}
			if (dst.first == pos.first - 2 && dst.second == pos.second - 1)
			{
				return detail::square_empty_or_enemy(*this, dst, board, allow_invulnerable);
			}
		}
		else
		{
			if (dst.first == pos.first - 1 && dst.second == pos.second + 2)
			{
				return detail::square_empty_or_enemy(*this, dst, board, allow_invulnerable);
			}
			if (dst.first == pos.first - 2 && dst.second == pos.second + 1)
			{
				return detail::square_empty_or_enemy(*this, dst, board, allow_invulnerable);
			}
		}
	}
	else
	{
		if (dst.second < pos.second)
		{
			if (dst.first == pos.first + 1 && dst.second == pos.second - 2)
			{
				return detail::square_empty_or_enemy(*this, dst, board, allow_invulnerable);
			}
			if (dst.first == pos.first + 2 && dst.second == pos.second - 1)
			{
				return detail::square_empty_or_enemy(*this, dst, board, allow_invulnerable);
			}
		}
		else
		{
			if (dst.first == pos.first + 1 && dst.second == pos.second + 2)
			{
				return detail::square_empty_or_enemy(*this, dst, board, allow_invulnerable);
			}
			if (dst.first == pos.first + 2 && dst.second == pos.second + 1)
			{
				return detail::square_empty_or_enemy(*this, dst, board, allow_invulnerable);
			}
		}
	}
	return false;
}

std::vector<Move> Knight::all_moves(const Chessboard& board) const
{
	std::vector<Move> moves;
	using namespace detail;

	full_check_and_add(*this, {pos.first - 1, pos.second - 2}, board, moves);
	full_check_and_add(*this, {pos.first + 1, pos.second - 2}, board, moves);
	full_check_and_add(*this, {pos.first - 2, pos.second - 1}, board, moves);
	full_check_and_add(*this, {pos.first - 2, pos.second + 1}, board, moves);
	full_check_and_add(*this, {pos.first - 1, pos.second + 2}, board, moves);
	full_check_and_add(*this, {pos.first + 1, pos.second + 2}, board, moves);
	full_check_and_add(*this, {pos.first + 2, pos.second - 1}, board, moves);
	full_check_and_add(*this, {pos.first + 2, pos.second + 1}, board, moves);		
	
	return moves;
}

bool Pawn::can_move(Pos dst, const Chessboard& board, bool allow_invulnerable) const
{
	if (team() == Team::black)
	{
		if (dst.first == pos.first + 1 && dst.second == pos.second)
		{
			return detail::square_empty(dst, board);
		}
		if (!has_moved && dst.first == pos.first + 2 && dst.second == pos.second)
		{
			return detail::square_empty({dst.first - 1, dst.second}, board) && detail::square_empty(dst, board);
		}
		if ((dst.first == pos.first + 1) && (dst.second == pos.second + 1 || dst.second == pos.second - 1))
		{
			return detail::square_enemy(*this, dst, board, allow_invulnerable);
		}
	}
	else
	{
		if (dst.first == pos.first - 1 && dst.second == pos.second)
		{
			return detail::square_empty(dst, board);
		}
		if (!has_moved && dst.first == pos.first - 2 && dst.second == pos.second)
		{
			return detail::square_empty({dst.first + 1, dst.second}, board) && detail::square_empty(dst, board);
		}
		if ((dst.first == pos.first - 1) && (dst.second == pos.second + 1 || dst.second == pos.second - 1))
		{
			return detail::square_enemy(*this, dst, board, allow_invulnerable);
		}
	}
	return false;
}

std::vector<Move> Pawn::all_moves(const Chessboard& board) const
{
	std::vector<Move> moves;
	using namespace detail;

	if (team() == Team::white)
	{
		Pos dst{pos.first - 1, pos.second};
		if (square_exists(dst) && square_empty(dst, board))
		{
			moves.emplace_back(Move{pos, dst});
			dst = {pos.first - 2, pos.second};
			if (!has_moved && square_exists(dst) && square_empty(dst, board))
			{
				moves.emplace_back(Move{pos, dst});
			}
		}
		dst = {pos.first - 1, pos.second + 1};
		if (square_exists(dst) && square_enemy(*this, dst, board))
		{
			moves.emplace_back(Move{pos, dst});
		}
		dst = {pos.first - 1, pos.second - 1};
		if (square_exists(dst) && square_enemy(*this, dst, board))
		{
			moves.emplace_back(Move{pos, dst});
		}
	}
	else
	{
		Pos dst{pos.first + 1, pos.second};
		if (square_exists(dst) && square_empty(dst, board))
		{
			moves.emplace_back(Move{pos, dst});
			dst = {pos.first + 2, pos.second};
			if (!has_moved && square_exists(dst) && square_empty(dst, board))
			{
				moves.emplace_back(Move{pos, dst});
			}
		}
		dst = {pos.first + 1, pos.second + 1};
		if (square_exists(dst) && square_enemy(*this, dst, board))
		{
			moves.emplace_back(Move{pos, dst});
		}
		dst = {pos.first + 1, pos.second - 1};
		if (square_exists(dst) && square_enemy(*this, dst, board))
		{
			moves.emplace_back(Move{pos, dst});
		}
	}

	return moves;
}

namespace detail
{

	bool check_diagonal(const Piece& piece, Pos dst, const Chessboard& board, bool allow_invulnerable)
	{
		auto di = static_cast<int>(dst.first) - static_cast<int>(piece.pos.first);
		auto dj = static_cast<int>(dst.second) - static_cast<int>(piece.pos.second);
		if (std::abs(di) != std::abs(dj))
		{
			return false;
		}
		auto i_forward = di > 0;
		auto j_forward = dj > 0;
		for (auto i = piece.pos.first, j = piece.pos.second; i != dst.first; i_forward ? ++i : --i, j_forward ? ++j : --j)
		{
			// Skip starting pos
			if (i == piece.pos.first)
			{
				continue;
			}
			if (board.piece_at(i, j) != nullptr)
			{
				return false;
			}
		}
		return square_empty_or_enemy(piece, dst, board, allow_invulnerable);
	}

	bool check_straight(const Piece& piece, Pos dst, const Chessboard& board, bool allow_invulnerable)
	{
		bool horizontal = piece.pos.first == dst.first && piece.pos.second != dst.second;
		bool vertical = piece.pos.first != dst.first && piece.pos.second == dst.second;
		if (!horizontal && !vertical)
		{
			return false;
		}
		if (horizontal)
		{
			bool forward = (dst.second - piece.pos.second) > 0;
			for (auto i = piece.pos.second; i != dst.second; forward ? ++i : --i)
			{
				if (i == piece.pos.second)
				{
					continue;
				}
				if (board.piece_at(dst.first, i) != nullptr)
				{
					return false;
				}
			}
		}
		else
		{
			bool forward = (dst.first - piece.pos.first) > 0;
			for (auto i = piece.pos.first; i != dst.first; forward ? ++i : --i)
			{
				if (i == piece.pos.first)
				{
					continue;
				}
				if (board.piece_at(i, dst.second) != nullptr)
				{
					return false;
				}
			}
		}
		return square_empty_or_enemy(piece, dst, board, allow_invulnerable);
	}

	bool square_empty_or_enemy(const Piece& piece, Pos dst, const Chessboard& board, bool allow_invulnerable)
	{
		auto target = board.piece_at(dst);
		return (target == nullptr)
				? true
				: (target->team() != piece.team() && (allow_invulnerable || !target->invulnerable()));
	}

	bool square_empty(Pos dst, const Chessboard& board)
	{
		return (board.piece_at(dst) == nullptr);
	}
	
	bool square_enemy(const Piece& piece, Pos dst, const Chessboard& board, bool allow_invulnerable)
	{
		auto target = board.piece_at(dst);
		return (target == nullptr)
				? false
				: (target->team() != piece.team() && (allow_invulnerable || !target->invulnerable()));
	}
	
	bool square_exists(Pos dst)
	{
		return dst.first < Chessboard::size && dst.second < Chessboard::size;
	}

	void full_check_and_add(const Piece& piece, Pos dst, const Chessboard& board, std::vector<Move>& moves)
	{
		if (square_exists(dst) && square_empty_or_enemy(piece, dst, board))
		{
			moves.emplace_back(Move{piece.pos, dst});
		}
	}
	
	bool check_and_add(const Piece& piece, Pos dst, const Chessboard& board, std::vector<Move>& moves)
	{
		if (square_empty(dst, board))
		{
			moves.emplace_back(Move{piece.pos, dst});
			return true;
		}
		if (square_enemy(piece, dst, board))
		{
			moves.emplace_back(Move{piece.pos, dst});
		}
		return false;
	}

	void add_diagonals(const Piece& piece, const Chessboard& board, std::vector<Move>& moves)
	{
		for (unsigned char i = piece.pos.first + 1, j = piece.pos.second + 1;
				i < Chessboard::size && j < Chessboard::size;
				++i, ++j)
		{
			if (!check_and_add(piece, {i, j}, board, moves))
			{
				break;
			}
		}
		for (unsigned char i = piece.pos.first - 1, j = piece.pos.second - 1;
				i < Chessboard::size && j < Chessboard::size;
				--i, --j)
		{
			if (!check_and_add(piece, {i, j}, board, moves))
			{
				break;
			}
		}		
		for (unsigned char i = piece.pos.first + 1, j = piece.pos.second - 1;
				i < Chessboard::size && j < Chessboard::size;
				++i, --j)
		{
			if (!check_and_add(piece, {i, j}, board, moves))
			{
				break;
			}
		}		
		for (unsigned char i = piece.pos.first - 1, j = piece.pos.second + 1;
				i < Chessboard::size && j < Chessboard::size;
				--i, ++j)
		{
			if (!check_and_add(piece, {i, j}, board, moves))
			{
				break;
			}
		}			
	}
	
	void add_straights(const Piece& piece, const Chessboard& board, std::vector<Move>& moves)
	{
		for (unsigned char i = piece.pos.first + 1; i < Chessboard::size; ++i)
		{
			if (!check_and_add(piece, {i, piece.pos.second}, board, moves))
			{
				break;
			}			
		}		
		for (unsigned char i = piece.pos.first - 1; i < Chessboard::size; --i)
		{
			if (!check_and_add(piece, {i, piece.pos.second}, board, moves))
			{
				break;
			}	
		}		
		for (unsigned char i = piece.pos.second + 1; i < Chessboard::size; ++i)
		{
			if (!check_and_add(piece, {piece.pos.first, i}, board, moves))
			{
				break;
			}				
		}				
		for (unsigned char i = piece.pos.second - 1; i < Chessboard::size; --i)
		{
			if (!check_and_add(piece, {piece.pos.first, i}, board, moves))
			{
				break;
			}	
		}				
	}
	
}
