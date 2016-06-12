#include "Chess_decl.hpp"
#include <ostream>

std::ostream& operator<<(std::ostream& os, Team team)
{
	team == Team::white ? os << "white" : os << "black";
	return os;
}

std::ostream& operator<<(std::ostream& os, Result result)
{
	switch (result)
	{
		case Result::white_win:
			os << "white won!";
			break;
		case Result::black_win:
			os << "black won!";
			break;
		case Result::draw:
			os << "draw!";
			break;
		default: break;
	}
	return os;
}

std::ostream& operator<<(std::ostream& os, Pos pos)
{
	os << static_cast<char>(pos.second + 'a') << static_cast<char>('8' - pos.first);
	return os;
}

std::ostream& operator<<(std::ostream& os, Move move)
{
	os << move.src << " " << move.dst;
	return os;
}

bool operator==(Move lhs, Move rhs) noexcept
{
	return lhs.src == rhs.src && lhs.dst == rhs.dst;
}
