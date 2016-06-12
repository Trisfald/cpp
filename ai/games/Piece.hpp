#ifndef AI_GAMES_PIECE_HPP_
#define AI_GAMES_PIECE_HPP_

#include <iosfwd>
#include <vector>
#include "Chess_decl.hpp"

class Chessboard;
typedef unsigned char Code_type;

class Piece
{
public:
	Piece(Pos pos, Code_type code) : pos(pos), code(code) {}
	Piece(const Piece&) = default;
	virtual ~Piece() = default;
	
	Team team() const { return code < black_offset ? Team::white : Team::black; }
	void set_moved() { has_moved = true; }
	bool invulnerable() const { return (code & (black_offset - 1)) == 0; }
	virtual char get_symbol() const = 0;
	virtual bool can_move(Pos dst, const Chessboard& board, bool allow_invulnerable = false) const = 0;
	virtual std::vector<Move> all_moves(const Chessboard& board) const = 0;
	virtual float worth() const = 0;
	virtual Piece* clone() const = 0;

	static constexpr Code_type black_offset = 16;
	Pos pos;
	const Code_type code;
protected:
	bool has_moved = false;
};

class King : public Piece
{
public:
	using Piece::Piece;
	char get_symbol() const override { return code < black_offset ? 'k' : 'K'; }
	Piece* clone() const override { return new King(*this); }
	bool can_move(Pos dst, const Chessboard& board, bool allow_invulnerable) const override;
	std::vector<Move> all_moves(const Chessboard& board) const override;
	float worth() const override { return 1.0f; }
};

class Queen : public Piece
{
public:	
	using Piece::Piece;
	~Queen() override = default;
	char get_symbol() const override { return code < black_offset ? 'q' : 'Q'; }
	Piece* clone() const override { return new Queen(*this); }
	bool can_move(Pos dst, const Chessboard& board, bool allow_invulnerable) const override;
	std::vector<Move> all_moves(const Chessboard& board) const override;
	float worth() const override { return 9.0f; }
};

class Rook : public Piece
{
public:
	using Piece::Piece;
	~Rook() override = default;
	char get_symbol() const override { return code < black_offset ? 'r' : 'R'; }
	Piece* clone() const override { return new Rook(*this); }
	bool can_move(Pos dst, const Chessboard& board, bool allow_invulnerable) const override;
	std::vector<Move> all_moves(const Chessboard& board) const override;
	float worth() const override { return 5.0f; }
};

class Bishop : public Piece
{
public:
	using Piece::Piece;
	~Bishop() override = default;
	char get_symbol() const override { return code < black_offset ? 'b' : 'B'; }
	Piece* clone() const override { return new Bishop(*this); }
	bool can_move(Pos dst, const Chessboard& board, bool allow_invulnerable) const override;
	std::vector<Move> all_moves(const Chessboard& board) const override;
	float worth() const override { return 3.0f; }
};

class Knight : public Piece
{
public:
	using Piece::Piece;
	~Knight() override = default;
	char get_symbol() const override { return code < black_offset ? 'n' : 'N'; }
	Piece* clone() const override { return new Knight(*this); }
	bool can_move(Pos dst, const Chessboard& board, bool allow_invulnerable) const override;
	std::vector<Move> all_moves(const Chessboard& board) const override;
	float worth() const override { return 3.0f; }
};

class Pawn : public Piece
{
public:
	using Piece::Piece;
	~Pawn() override = default;
	char get_symbol() const override { return code < black_offset ? 'p' : 'P'; }
	Piece* clone() const override { return new Pawn(*this); }
	bool can_move(Pos dst, const Chessboard& board, bool allow_invulnerable) const override;
	std::vector<Move> all_moves(const Chessboard& board) const override;
	float worth() const override { return 1.0f; }
};

std::ostream& operator<<(std::ostream& os, const Piece& piece);

namespace detail
{

	bool check_diagonal(const Piece& piece, Pos dst, const Chessboard& board, bool allow_invulnerable = false);
	bool check_straight(const Piece& piece, Pos dst, const Chessboard& board, bool allow_invulnerable = false);
	bool square_empty_or_enemy(const Piece& piece, Pos dst, const Chessboard& board, bool allow_invulnerable = false);
	bool square_enemy(const Piece& piece, Pos dst, const Chessboard& board, bool allow_invulnerable = false);
	bool square_empty(Pos dst, const Chessboard& board);
	bool square_exists(Pos dst);
	void full_check_and_add(const Piece& piece, Pos dst, const Chessboard& board, std::vector<Move>& moves);
	void add_diagonals(const Piece& piece, const Chessboard& board, std::vector<Move>& moves);
	void add_straights(const Piece& piece, const Chessboard& board, std::vector<Move>& moves);
	bool check_and_add(const Piece& piece, Pos dst, const Chessboard& board, std::vector<Move>& moves);
	
}

#endif
