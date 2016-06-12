#ifndef AI_GAMES_CHESS_DECL_HPP_
#define AI_GAMES_CHESS_DECL_HPP_

#include <iosfwd>
#include <utility>

typedef std::pair<unsigned char, unsigned char> Pos;

struct Move
{
    Pos src;
    Pos dst;
};

bool operator==(Move, Move) noexcept;

enum class Result
{
	none, white_win, black_win, draw
};

enum class Team
{
	white, black
};

inline Team opponent_team(Team team)
{
	return team == Team::white ? Team::black : Team::white;
}

inline Result loss(Team team)
{
	return team == Team::white ? Result::black_win : Result::white_win;
}

std::ostream& operator<<(std::ostream& os, Team team);

std::ostream& operator<<(std::ostream& os, Result result);

std::ostream& operator<<(std::ostream& os, Pos pos);

std::ostream& operator<<(std::ostream& os, Move move);

namespace constants
{
	
	constexpr unsigned max_checks = 3;
	constexpr unsigned max_turns_no_capture = 50;
	
}

#endif
