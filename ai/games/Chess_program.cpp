#include "Chess_program.hpp"

#include <cctype>
#include <limits>
#include <stdexcept>

Team read_team(std::ostream& os, std::istream& is)
{
	char c;
	do
	{
		os << "Play as white or as black? [W/B]: ";
		is >> c;
		c = toupper(c);
		is.clear();
		is.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
	}
	while (c != 'W' && c != 'B');
	os << "\n\n\n";
	return (c == 'W') ? Team::white : Team::black;
}

std::ostream& operator<<(std::ostream& os, const Chess_program& rhs)
{
	if (rhs.turn() != 1)
	{
		rhs.print_turn_resolution(os);
	}
	os << "---------------------\n";
	os << "Turn: " << rhs.turn();
	os << "\n---------------------\n\n";
	os << rhs.board_ << "\n\n";
	rhs.print_bar(os);
	os << "\n";
	return os;
}

Chess_program::Chess_program(Team human)
{
	if (human == Team::white)
	{
		white_ = std::make_unique<Human>(Team::white);
		black_ = std::make_unique<AI>(Team::black);
	}
	else
	{
		white_ = std::make_unique<AI>(Team::white);
		black_ = std::make_unique<Human>(Team::black);
	}
}

void Chess_program::print_bar(std::ostream& os) const
{
	current_player().print_cmd(os);
}

void Chess_program::do_turn()
{
	last_move_ = current_player().do_turn(exit_, board_);
	if (exit_)
	{
		return;
	}
	else if (last_move_ == nullptr)
	{
		if (board_.checks(current_player().team) != 0)
		{
			result_ = loss(current_player().team);
		}
		else
		{
			result_ = Result::draw;
		}
	}
	else
	{
		last_capture_ = board_.apply_move(*last_move_);
		if (last_capture_ == nullptr)
		{
			++board_.turns_no_capture;
		}
		else
		{
			board_.turns_no_capture = 0;
		}
		after_turn();
	}
}

void Chess_program::after_turn()
{
	if (board_.turns_no_capture >= constants::max_turns_no_capture)
	{
		result_ = Result::draw;
	}
}

void Chess_program::print_turn_resolution(std::ostream& os) const
{
	os << "\n";
	if (last_move_ != nullptr)
	{
		os << "Move: " << *last_move_ << "\n";
	}
	if (last_capture_ != nullptr)
	{
		os << "Captured " << last_capture_->team() << " " << *last_capture_ << "\n";
	}
	if (board_.checks(Team::white))
	{
		os << "White king is in check!\n";
	}
	if (board_.checks(Team::black))
	{
		os << "Black king is in check!\n";
	}
	os << "\n";
}

void Human::print_cmd(std::ostream& os) const
{
	os << "Your turn:\n     Q - quit, [square] [square] - move";
}

std::unique_ptr<Move> Human::do_turn(bool& exit, const Chessboard& board)
{
	while (true)
	{
		std::string cmd;
		try
		{
			std::getline(is_, cmd);
			if (cmd == "Q" || cmd == "q")
			{
				exit = true;
				return nullptr;
			}
			auto move = move_from_cmd(cmd);
			if (is_move_legal(move, team, board))
			{
				return std::make_unique<decltype(move)>(move);
			}
			else
			{
				os_ << "Illegal move\n";
			}
			os_ << "\n";
		}
		catch (const std::invalid_argument& e)
		{
			os_ << e.what() << "\n";
		}
	}
}

void AI::print_cmd(std::ostream& os) const
{
	os << "Wait for other player's move...";
}

std::unique_ptr<Move> AI::do_turn(bool&, const Chessboard& board)
{
	return solver_(board);
}
