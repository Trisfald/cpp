#ifndef AI_GAMES_CHESSBOARD_HPP_
#define AI_GAMES_CHESSBOARD_HPP_

#include <iosfwd>
#include <array>
#include <utility>
#include <memory>
#include "Piece.hpp"
#include "Chess_decl.hpp"

class Chessboard;
struct Piece;

std::ostream& operator<<(std::ostream&, const Chessboard&);
bool is_move_legal(Move, Team, const Chessboard&);
bool is_move_in_check(Move, Team, const Chessboard&);

class Chessboard
{
	friend std::ostream& operator<<(std::ostream&, const Chessboard&);
	friend bool is_move_legal(Move, Team, const Chessboard&);
	friend bool is_move_in_check(Move, Team, const Chessboard&);
	typedef std::array<Piece*, 32> Pieces;
	typedef std::pair<Pieces::iterator, Pieces::iterator> Team_it_pair;
public:
	enum Chessman : Code_type;
	Chessboard();
	Chessboard(const Chessboard& rhs);
	Chessboard(Chessboard&& rhs) noexcept = default;
	Chessboard& operator=(const Chessboard&) = delete;
	~Chessboard();

	void add_piece(Piece* piece_ptr);
	void remove_piece(Code_type code);
	std::unique_ptr<Piece> apply_move(Move move);
	inline Piece* piece_at(unsigned char i, unsigned char j) const;
	inline Piece* piece_at(Pos pos) const;
	bool is_king_in_check(Team team) const;
	Chessboard successor(Move move) const;
	unsigned turn() const { return turn_; }
	unsigned& checks(Team team) { return (team == Team::white) ? white_checks_ : black_checks_; }
	unsigned checks(Team team) const { return (team == Team::white) ? white_checks_ : black_checks_; }
	Team current_team() const { return (turn_ & 1u) ? Team::white : Team::black; }
	inline Team_it_pair get_team_pieces(Team team) const;
	const auto& history() const { return history_; }
	
	static constexpr Code_type empty = 255;
	static constexpr unsigned size = 8;
	static constexpr unsigned opening_length = 5;

	unsigned turns_no_capture = 0;
private:
	typedef std::array<std::array<Code_type, size>, size> Board;
		
	void init_color(unsigned front_row, unsigned back_row, unsigned piece_offset);
	std::unique_ptr<Piece> move_piece(Code_type code, Pos dst);

	Board board_;
	Pieces pieces_;
	Team_it_pair white_it_pair_;
	Team_it_pair black_it_pair_;
	unsigned turn_ = 1;
	unsigned white_checks_ = 0;
	unsigned black_checks_ = 0;
	std::array<Move, opening_length> history_;
};

Move move_from_cmd(const std::string& cmd);

Pos pos_from_chars(char i, char j);

Piece* Chessboard::piece_at(unsigned char i, unsigned char j) const 
{
	const auto& code = board_[i][j];
	return code == empty ? nullptr : pieces_[code];
}

Piece* Chessboard::piece_at(Pos pos) const
{
	const auto& code = board_[pos.first][pos.second];
	return code == empty ? nullptr : pieces_[code];
}

typename Chessboard::Team_it_pair
Chessboard::get_team_pieces(Team team) const
{
	return team == Team::white ? white_it_pair_ : black_it_pair_;
}

#endif
