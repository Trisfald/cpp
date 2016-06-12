#include "Chess_AI.hpp"
#include "Chessboard.hpp"
#include <string>
#include <iterator>
#include <algorithm>
#include <chrono>

Move_generator::Move_generator() : re_(std::chrono::system_clock::now().time_since_epoch().count())
{}

std::vector<Move> Move_generator::operator()(const Chessboard& board)
{
	std::vector<Move> moves;
	if (board.turn() < Chessboard::opening_length + 2)
	{
		auto it_pair = openings_.get_available_moves(board.history(), board.turn());
		auto distance = std::distance(it_pair.first, it_pair.second);
		if (distance > 0)
		{
			std::uniform_int_distribution<decltype(distance)> dist(0, distance - 1);
			std::advance(it_pair.first, dist(re_));
			if (!is_move_in_check((*it_pair.first)->move, board.current_team(), board))
			{
				moves.emplace_back((*it_pair.first)->move);
			}
		}
	}
	if (moves.empty())
	{
		for (auto pieces_it = board.get_team_pieces(board.current_team());
				pieces_it.first != pieces_it.second;
				++pieces_it.first)
		{
			if (*pieces_it.first == nullptr)
			{
				continue;
			}
			for (const auto move : (*pieces_it.first)->all_moves(board))
			{
				if (!is_move_in_check(move, board.current_team(), board))
				{
					moves.emplace_back(move);
				}
			}
		}
	}
	return moves;
}

Search_interrupt Interrupt_tester::operator()(const Chessboard&, unsigned depth) const
{
    if (depth > max_depth_)
    {
        return Search_interrupt::cutoff;
    }
    return Search_interrupt::none;
}

float Evaluator::operator()(const Chessboard& board, Search_interrupt) const
{
	float score = 0.0f;
	auto pieces_it = board.get_team_pieces(team_);
	std::for_each(pieces_it.first, pieces_it.second, [&score] (auto it) { if (it != nullptr) score += it->worth(); });
	pieces_it = board.get_team_pieces(opponent_team(team_));
	std::for_each(pieces_it.first, pieces_it.second, [&score] (auto it) { if (it != nullptr) score -= it->worth(); });

	if (board.is_king_in_check(opponent_team(team_)))
	{
		score += 5.0f;
	}
	if (board.is_king_in_check(team_))
	{
		score -= 5.0f;
	}
	return score;
}

bool operator==(const Move_node_ptr& lhs, Move rhs) noexcept
{
	return lhs->move == rhs;
}

bool operator==(const Move_node_ptr& lhs, const Move_node_ptr& rhs) noexcept
{
	return lhs->move == rhs->move;
}

Openings::Openings()
{
	using namespace detail;
	(*this)[make_node("e2 e4")][make_node("e7 e5")][make_node("g1 f3")];
	(*this)[make_node("e2 e4")][make_node("e7 e5")][make_node("f2 f4")];
	(*this)[make_node("e2 e4")][make_node("e7 e5")][make_node("b1 c3")];
	(*this)[make_node("e2 e4")][make_node("c7 c5")][make_node("g1 f3")];
	(*this)[make_node("e2 e4")][make_node("e7 e6")][make_node("d2 d4")];
	(*this)[make_node("d2 d4")][make_node("d7 d5")][make_node("c2 c4")];
	(*this)[make_node("d2 d4")][make_node("d7 d5")][make_node("g1 f3")];
	(*this)[make_node("d2 d4")][make_node("g8 f6")][make_node("c2 c4")];
}

namespace detail
{

	Move_node_ptr make_node(const std::string& cmd)
	{
		return std::make_shared<Move_node>(move_from_cmd(cmd));
	}

}
