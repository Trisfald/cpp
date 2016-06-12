#include "Chessboard.hpp"
#include <cctype>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <iostream>

enum Chessboard::Chessman : Code_type
{
	w_king = 0,
	w_queen = 1,
	w_rook_1 = 2,
	w_rook_2 = 3,
	w_bishop_1 = 4,
	w_bishop_2 = 5,
	w_knight_1 = 6,
	w_knight_2 = 7,
	w_pawn_1 = 8,
	w_pawn_2 = 9,
	w_pawn_3 = 10,
	w_pawn_4 = 11,
	w_pawn_5 = 12,
	w_pawn_6 = 13,
	w_pawn_7 = 14,
	w_pawn_8 = 15,
	b_king = Piece::black_offset,
	b_queen = Piece::black_offset + 1,
	b_rook_1 = Piece::black_offset + 2,
	b_rook_2 = Piece::black_offset + 3,
	b_bishop_1 = Piece::black_offset + 4,
	b_bishop_2 = Piece::black_offset + 5,
	b_knight_1 = Piece::black_offset + 6,
	b_knight_2 = Piece::black_offset + 7,
	b_pawn_1 = Piece::black_offset + 8,
	b_pawn_2 = Piece::black_offset + 9,
	b_pawn_3 = Piece::black_offset + 10,
	b_pawn_4 = Piece::black_offset + 11,
	b_pawn_5 = Piece::black_offset + 12,
	b_pawn_6 = Piece::black_offset + 13,
	b_pawn_7 = Piece::black_offset + 14,
	b_pawn_8 = Piece::black_offset + 15
};

Chessboard::Chessboard()
{
	std::fill(&board_[0][0], &board_[0][0] + sizeof(board_), static_cast<Code_type>(empty));
	init_color(Chessboard::size - 2, Chessboard::size - 1, 0);
	init_color(1, 0, Piece::black_offset);
	white_it_pair_ = std::make_pair(pieces_.begin(), pieces_.begin() + Piece::black_offset);
	black_it_pair_ = std::make_pair(pieces_.begin() + Piece::black_offset, pieces_.end());
}

Chessboard::Chessboard(const Chessboard& rhs)
: turns_no_capture(rhs.turns_no_capture),
		board_(rhs.board_),
		turn_(rhs.turn_),
		white_checks_(rhs.white_checks_),
		black_checks_(rhs.black_checks_),
		history_(rhs.history_)
{
	for (unsigned i = 0; i < rhs.pieces_.size(); ++i)
	{
		if (rhs.pieces_[i] == nullptr)
		{
			pieces_[i] = nullptr;
		}
		else
		{
			pieces_[i] = rhs.pieces_[i]->clone();
		}
	}
	white_it_pair_ = std::make_pair(pieces_.begin(),
			pieces_.begin() + (rhs.white_it_pair_.second - rhs.pieces_.begin()));
	black_it_pair_ = std::make_pair(pieces_.begin() + Piece::black_offset,
			pieces_.begin() + (rhs.black_it_pair_.second - rhs.pieces_.begin()));
}

Chessboard::~Chessboard()
{
	for (auto p : pieces_)
	{
		if (p != nullptr)
		{
			delete p;
		}
	}
}

void Chessboard::init_color(unsigned front_row, unsigned back_row, unsigned piece_offset)
{
	for (unsigned i = 0; i < Chessboard::size; ++i)
	{
		add_piece(new Pawn({front_row, i}, piece_offset + w_pawn_1 + i));
	}
	add_piece(new Rook({back_row, 0}, piece_offset + w_rook_1));
	add_piece(new Rook({back_row, 7}, piece_offset + w_rook_2));
	add_piece(new Knight({back_row, 1}, piece_offset + w_knight_1));
	add_piece(new Knight({back_row, 6}, piece_offset + w_knight_2));
	add_piece(new Bishop({back_row, 2}, piece_offset + w_bishop_1));
	add_piece(new Bishop({back_row, 5}, piece_offset + w_bishop_2));
	add_piece(new Queen({back_row, 3}, piece_offset + w_queen));
	add_piece(new King({back_row, 4}, piece_offset + w_king));
}

bool is_move_legal(Move move, Team team, const Chessboard& board)
{
	auto piece_ptr = board.piece_at(move.src);
	if (piece_ptr == nullptr)
	{
		return false;
	}
	if (piece_ptr->team() != team)
	{
		return false;
	}
	if (!piece_ptr->can_move(move.dst, board))
	{
		return false;
	}
	Chessboard next_state(board);
	next_state.move_piece(piece_ptr->code, move.dst);
	return !next_state.is_king_in_check(team);
}

bool is_move_in_check(Move move, Team team, const Chessboard& board)
{
	Chessboard next_state(board);
	next_state.move_piece(next_state.piece_at(move.src)->code, move.dst);
	return next_state.is_king_in_check(team);
}

bool Chessboard::is_king_in_check(Team team) const
{
	auto pieces_it = get_team_pieces(opponent_team(team));
	auto dst = pieces_[team == Team::white ? Chessboard::w_king : Chessboard::b_king]->pos;
	while (pieces_it.first != pieces_it.second)
	{
		if (*pieces_it.first != nullptr && (*pieces_it.first)->can_move(dst, *this, true))
		{
			return true;
		}
		++pieces_it.first;
	}
	return false;
}

std::unique_ptr<Piece> Chessboard::move_piece(Code_type code, Pos dst)
{
	auto piece_ptr = pieces_[code];
	piece_ptr->set_moved();
	// Capture
	auto target_ptr = piece_at(dst);
	std::unique_ptr<Piece> capture;
	if (target_ptr != nullptr)
	{
		capture.reset(target_ptr->clone());
		remove_piece(target_ptr->code);
	}
	// Apply the move
	board_[piece_ptr->pos.first][piece_ptr->pos.second] = empty;
	piece_ptr->pos = dst;
	board_[dst.first][dst.second] = piece_ptr->code;
	return capture;
}

void Chessboard::add_piece(Piece* piece_ptr)
{
	board_[piece_ptr->pos.first][piece_ptr->pos.second] = piece_ptr->code;
	pieces_[piece_ptr->code] = piece_ptr;
}

void Chessboard::remove_piece(Code_type code)
{
	auto piece_ptr = pieces_[code];
	delete piece_ptr;
	pieces_[code] = nullptr;
}	

Chessboard Chessboard::successor(Move move) const
{
	Chessboard ret(*this);
	ret.apply_move(move);
	return ret;
}

std::unique_ptr<Piece> Chessboard::apply_move(Move move)
{
	auto capture = move_piece(piece_at(move.src)->code, move.dst);
	if (is_king_in_check(opponent_team(current_team())))
	{
		++checks(opponent_team(current_team()));
	}
	else
	{
		checks(opponent_team(current_team())) = 0;
	}
	if (turn_++ <= Chessboard::opening_length)
	{
		history_[turn_ - 2] = move;
	}
	return capture;
}

std::ostream& operator<<(std::ostream& os, const Chessboard& rhs)
{
	os << "   ";
	for (unsigned i = 0; i < Chessboard::size; ++i)
	{
		os << static_cast<char>('a' + i) << ' ';
	}
	os << "  \n\n" ;
	for (unsigned i = 0; i < Chessboard::size; ++i)
	{
		for (int j = -1; j < static_cast<int>(Chessboard::size) + 1; ++j)
		{
			if (j == -1)
			{
				os << Chessboard::size - i << "  ";
			}
			else if (j == Chessboard::size)
			{
				os << " " << Chessboard::size - i;
			}
			else
			{
				auto piece = rhs.piece_at(i, j);
				if (piece != nullptr)
				{
					os << *piece;
				}
				else
				{
					os << ".";
				}
				os << " ";
			}
		}
		os << "\n";
	}

	os << "\n   ";
	for (unsigned i = 0; i < Chessboard::size; ++i)
	{
		os << static_cast<char>('a' + i) << ' ';
	}
	os << "  \n" ;
	return os;
}

Move move_from_cmd(const std::string& cmd)
{
    if (cmd.size() != 5)
    {
    	throw std::invalid_argument("Invalid command string");
    }
    Pos src = pos_from_chars(tolower(cmd[0]), tolower(cmd[1]));
    Pos dest = pos_from_chars(tolower(cmd[3]), tolower(cmd[4]));
    if (src == dest)
    {
    	throw std::invalid_argument("Null move");
    }
    return Move{src, dest};
}

Pos pos_from_chars(char i, char j)
{
    Pos pos;
    if (i < 'a' || j > static_cast<char>('a' + Chessboard::size))
    {
    	throw std::invalid_argument("Invalid column");
    }
    else
    {
    	pos.second = i - 'a';
    }
    if (j < '1' || j > static_cast<char>('0' + Chessboard::size))
    {
    	throw std::invalid_argument("Invalid row");
    }
    else
    {
    	pos.first = '0' + Chessboard::size - j;
    }
    return pos;
}
