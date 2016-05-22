#ifndef AI_SEARCHING_PUZZLE_BOARD_HPP_
#define AI_SEARCHING_PUZZLE_BOARD_HPP_

#include <array>
#include <iostream>
#include <iomanip>
#include <cstddef>
#include <algorithm>
#include <vector>
#include <utility>
#include <tuple>

template <signed char N> class Puzzle_board;
template <signed char N> std::ostream& operator<<(std::ostream& os, const Puzzle_board<N>& rhs);

template <signed char N>
class Puzzle_board
{
	template <signed char U>
	friend std::ostream& operator<<(std::ostream& os, const Puzzle_board<U>& rhs);
public:
	typedef std::array<signed char, N> Row;
	typedef std::array<Row, N> Data;
	enum { size = N };

	Puzzle_board();
	Puzzle_board(const Puzzle_board&) = default;
	Puzzle_board(Puzzle_board&& rhs) noexcept = default;
	Puzzle_board(Data&& data) noexcept : data_(std::move(data)) {}

	Puzzle_board& operator=(const Puzzle_board&) = default;
	Row& operator[](std::size_t n)
	{
		return data_[n];
	}
	const Row& operator[](std::size_t n) const
	{
		return data_[n];
	}
	const Data& data() const
	{
		return data_;
	};
private:
	Data data_;
};

struct Puzzle_action
{
	enum Type : char
	{
		IDLE, UP, DOWN, RIGHT, LEFT
	};

	Puzzle_action() = default;
	Puzzle_action(Type type) : value(type) {}
	inline Puzzle_action get_reverse() const;

	Type value = IDLE;
};

Puzzle_action Puzzle_action::get_reverse() const
{
	switch (value)
	{
		case UP:
			return Puzzle_action(DOWN);
		case DOWN:
			return Puzzle_action(UP);
		case RIGHT:
			return Puzzle_action(LEFT);
		case LEFT:
			return Puzzle_action(RIGHT);
		default:
			return Puzzle_action(IDLE);
	}
}

std::ostream& operator<<(std::ostream& os, const Puzzle_action& rhs)
{
	switch (rhs.value)
	{
		case Puzzle_action::IDLE:
			os << "IDLE";
			break;
		case Puzzle_action::UP:
			os << "UP";
			break;
		case Puzzle_action::DOWN:
			os << "DOWN";
			break;
		case Puzzle_action::RIGHT:
			os << "RIGHT";
			break;
		case Puzzle_action::LEFT:
			os << "LEFT";
			break;
	}
	return os;
}

template <signed char N>
struct Puzzle_successors_gen
{
	typedef std::pair<signed char, signed char> Pos;
	typedef std::tuple<Puzzle_board<N>, Puzzle_action, float> Successor;
	typedef void (*Insert_fn)(std::vector<Successor>& v,
			typename Puzzle_board<N>::Data&& data,
			Puzzle_action::Type action,
			unsigned& i);
	auto operator()(const Puzzle_board<N>& parent) const
	{
		std::vector<Successor> v;
		Pos zero = find_zero(parent);
		unsigned i = 0;
		static auto fn = [](std::vector<Successor>& v,
				typename Puzzle_board<N>::Data&& data,
				Puzzle_action::Type action,
				unsigned& i)
				{ v.emplace_back(std::forward_as_tuple(std::move(data), Puzzle_action(action), 1.0f)); };
		check_and_add(v, parent, zero, {zero.first+1, zero.second}, fn, i, Puzzle_action::DOWN);
		check_and_add(v, parent, zero, {zero.first-1, zero.second}, fn, i, Puzzle_action::UP);
		check_and_add(v, parent, zero, {zero.first, zero.second+1}, fn, i, Puzzle_action::RIGHT);
		check_and_add(v, parent, zero, {zero.first, zero.second-1}, fn, i, Puzzle_action::LEFT);
		return v;
	}
private:
	void check_and_add(std::vector<Successor>& v,
			const Puzzle_board<N>& parent,
			const Pos& zero,
			const Pos& pos,
			Insert_fn insert_fn,
			unsigned& i,
			Puzzle_action::Type action) const noexcept
	{
		if (pos.first < 0 || pos.first >= N || pos.second < 0 || pos.second >= N)
		{
			return;
		}
		typename Puzzle_board<N>::Data data = parent.data();
		std::swap(data[zero.first][zero.second], data[pos.first][pos.second]);
		insert_fn(v, std::move(data), action, i);
	}
	Pos find_zero(const Puzzle_board<N>& parent) const noexcept
	{
		Pos zero;
		for (unsigned i = 0; i < N; ++i)
		{
			for (unsigned j = 0; j < N; ++j)
			{
				if (parent[i][j] == 0)
				{
					zero = {i, j};
					break;
				}
			}
		}
		return zero;
	}
};

template <signed char N>
struct Puzzle_heuristic
{
	float operator()(const Puzzle_board<N>& state, const Puzzle_board<N>& goal) const noexcept
	{
		float n = 0;
		for (unsigned i = 0; i < N; ++i)
		{
			for (unsigned j = 0; j < N; ++j)
			{
				if (state[i][j] != goal[i][j])
					++n;
			}
		}
		return n;
	}
};

template <signed char N>
struct Puzzle_heuristic_manhattan
{
	typedef std::pair<signed char, signed char> Pos;
	Puzzle_heuristic_manhattan(const Puzzle_board<N>& goal) noexcept
	{
		for (unsigned i = 0; i < N; ++i)
		{
			for (unsigned j = 0; j < N; ++j)
			{
				goal_positions_[goal[i][j]] = {i, j};
			}
		}
	}
	float operator()(const Puzzle_board<N>& state, const Puzzle_board<N>& goal) const noexcept
	{
		float n = 0;
		for (signed char i = 0; i < N; ++i)
		{
			for (signed char j = 0; j < N; ++j)
			{
				const auto& goal_pos = goal_positions_[state[i][j]];
				n += std::abs(goal_pos.first - i);
				n += std::abs(goal_pos.second - j);
			}
		}
		return n;
	}
private:
	std::array<Pos, N*N> goal_positions_;
};

template <signed char N>
Puzzle_board<N>::Puzzle_board()
{
	std::array<signed char, N*N> temp;
	for (std::size_t i = 0; i < N*N; ++i)
	{
		temp[i] = i;
	}
	std::random_shuffle(temp.begin(), temp.end());
	for (std::size_t i = 0; i < N*N; ++i)
	{
		data_[i / (N*N)][i % (N*N)] = temp[i];
	}
}

template <signed char N>
std::ostream& operator<<(std::ostream& os, const Puzzle_board<N>& rhs)
{
	for (auto ity = rhs.data_.cbegin(); ity != rhs.data_.cend(); ++ity)
	{
		for (auto itx = ity->cbegin(); itx != ity->cend(); ++itx)
		{
			os << std::setfill (' ') << std::setw(3) << static_cast<unsigned>(*itx);
		}
		os << std::endl;
	}
	return os;
}

template <signed char N>
bool operator==(const Puzzle_board<N>& lhs, const Puzzle_board<N>& rhs) noexcept
{
	return lhs.data() == rhs.data();
}

template <class T>
inline void hash_combine(std::size_t& seed, const T& v) noexcept
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{

	template <>
	template <signed char N>
	struct hash<Puzzle_board<N>>
	{
		std::size_t operator()(const Puzzle_board<N>& x) const noexcept
		{
			std::size_t seed = 0;
			for (auto i = x.data().cbegin(); i < x.data().cend(); ++i)
			{
				for (auto j = i->cbegin(); j < i->cend(); ++j)
				{
					hash_combine(seed, *j);
				}
			}
			return seed;
		}
	};

}

#endif
