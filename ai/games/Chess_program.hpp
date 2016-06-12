#ifndef AI_GAMES_CHESS_PROGRAM_HPP_
#define AI_GAMES_CHESS_PROGRAM_HPP_

#include <iostream>
#include <memory>
#include "Chess_decl.hpp"
#include "Chessboard.hpp"
#include "Alpha_beta.hpp"
#include "Chess_AI.hpp"

class Chess_program;
class Player;
std::ostream& operator<<(std::ostream&, const Chess_program&);

Team read_team(std::ostream&, std::istream&);

class Chess_program
{
	friend std::ostream& operator<<(std::ostream&, const Chess_program&);
public:
	explicit Chess_program(Team human);
	Chess_program(Chess_program&) = delete;
	Chess_program& operator=(Chess_program&) = delete;

	void do_turn();
	Player& current_player() const { return (turn() & 1u) ? *white_ : *black_; }
	Player& opponent_player() const { return (turn() & 1u) ? *black_ : *white_; }
	bool exit() const {	return exit_; }
	Result result() const {	return result_;	}
private:
	void print_bar(std::ostream&) const;
	void print_turn_resolution(std::ostream&) const;
	void after_turn();
	unsigned turn() const { return board_.turn(); }

	Chessboard board_;
	Result result_ = Result::none;
	bool exit_ = false;
	std::unique_ptr<Player> white_;
	std::unique_ptr<Player> black_;
	std::unique_ptr<Piece> last_capture_;
	std::unique_ptr<Move> last_move_;
};

class Player
{
public:
	explicit Player(Team team) : team(team) {}
	Player(Player&) = delete;
	Player& operator=(Player&) = delete;
	virtual ~Player() = default;
	virtual void print_cmd(std::ostream&) const = 0;
	virtual std::unique_ptr<Move> do_turn(bool& exit, const Chessboard& board) = 0;

	const Team team;
};

class Human : public Player
{
public:
	explicit Human(Team team, std::istream& is = std::cin, std::ostream& os = std::cout)
	: Player(team), is_(is), os_(os) {}
	void print_cmd(std::ostream&) const override;
	std::unique_ptr<Move> do_turn(bool& exit, const Chessboard& board) override;
private:
	std::istream& is_;
	std::ostream& os_;
};

class AI : public Player
{
public:
	explicit AI(Team team) : Player(team), solver_(Move_generator(), Interrupt_tester(6), Evaluator(team)) {}
	void print_cmd(std::ostream&) const override;
	std::unique_ptr<Move> do_turn(bool& exit, const Chessboard& board) override;
private:
	typedef Alfa_beta_search<Chessboard, Move, Move_generator, Interrupt_tester, Evaluator> Solver;
	Solver solver_;
};

#endif
