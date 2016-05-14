#ifndef AI_SEARCHING_N_QUEENS_HPP_
#define AI_SEARCHING_N_QUEENS_HPP_

#include <array>
#include <vector>
#include <random>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <cstddef>
#include <algorithm>

template <unsigned N> class N_queens;
template <unsigned N> struct Move_piece_generator;
template <unsigned N> struct Mutagen;
template <unsigned N> struct Pop_generator;
template <unsigned N> std::ostream& operator<<(std::ostream&, const N_queens<N>&);

template <unsigned N>	
class N_queens
{
	template <unsigned Y> friend std::ostream& operator<<(std::ostream&, const N_queens<Y>&);
	template <unsigned Y> friend struct Move_piece_generator;
	template <unsigned Y> friend struct Mutagen;
	template <unsigned Y> friend struct Pop_generator;
public:
	typedef decltype(N) Size_type;
	typedef std::pair<Size_type, Size_type> Queen;
	enum { size = N };

	N_queens() = default;
	N_queens(const N_queens& rhs) : board_(rhs.board_), queens_(rhs.queens_) {}
	N_queens(N_queens&&) noexcept = default;
	
	const std::array<bool, N> operator[](std::size_t n) const { return board_[n]; }
	N_queens& operator=(const N_queens&) = default;
	N_queens& operator=(N_queens&&) noexcept = default;
	
	inline void add_queen(const Queen& queen);
	inline void remove_queen(std::size_t n);
	bool goal() const;
	std::size_t num_queens() const { return queens_.size(); }
	std::vector<Size_type> get_free_columns() const;
private:
	typedef std::array<std::array<bool, N>, N> Board;

	Board board_ = {{{{0}}}};
	std::vector<Queen> queens_;
};

template <unsigned N>
bool N_queens<N>::goal() const 
{
	if (queens_.size() != N)
		return false;
	for (auto it = queens_.cbegin(); it != queens_.cend(); ++it)
	{
		// Straight checks
		for (Size_type i = 0; i < N; ++i)
		{
			if (board_[i][it->second] && i != it->first)
				return false;
		}
		for (Size_type j = 0; j < N; ++j)
		{
			if (board_[it->first][j] && j != it->second)
				return false;
		}
		// Diagonal checks
		for (Size_type i = it->first, j = it->second; i > 0 && j > 0; --i, --j)
		{
			if (board_[i-1][j-1])
				return false;
		}
		for (Size_type i = it->first, j = it->second; i < N-1 && j < N-1; ++i, ++j)
		{
			if (board_[i+1][j+1])
				return false;
		}
		for (Size_type i = it->first, j = it->second; i > 0 && j < N-1; --i, ++j)
		{
			if (board_[i-1][j+1])
				return false;
		}
		for (Size_type i = it->first, j = it->second; i < N-1 && j > 0; ++i, --j)
		{
			if (board_[i+1][j-1])
				return false;
		}
	}
	return true;	
}

template <unsigned N>
void N_queens<N>::add_queen(const Queen& queen)
{
	board_[queen.first][queen.second] = true;
	queens_.push_back(queen);
}

template <unsigned N>
void N_queens<N>::remove_queen(std::size_t n)
{
	board_[queens_[n].first][queens_[n].second] = false;
	queens_.erase(queens_.cbegin() + n);
}

template <unsigned N>
std::vector<typename N_queens<N>::Size_type> N_queens<N>::get_free_columns() const
{
	std::vector<N_queens<N>::Size_type> result;
	for (Size_type i = 0; i < N; ++i)
	{
		result.push_back(i);
	}
	for (const auto& queen : queens_)
	{
		result[queen.second] = N+1;
	}
	result.erase(std::remove(result.begin(), result.end(), N+1), result.end());

	return result;
}
	
template <unsigned N>
std::ostream& operator<<(std::ostream& os, const N_queens<N>& rhs)
{
	for (auto ity = rhs.board_.cbegin(); ity != rhs.board_.cend(); ++ity)
	{
		for (auto itx = ity->cbegin(); itx != ity->cend(); ++itx)
		{
			std::cout << std::setfill (' ') << std::setw(3) << ((*itx) ? 'o' : '.');
		}
		std::cout << std::endl;
	}
	return os;
}

template <unsigned N, typename Random_engine>
N_queens<N> random_queens_configuration(Random_engine& re)
{
	N_queens<N> res;
	std::uniform_int_distribution<typename N_queens<N>::Size_type> d(0, N-1);
	for (typename N_queens<N>::Size_type i = 0; i < N; ++i)
	{
		typename N_queens<N>::Queen queen = {d(re), i};
		res.add_queen(queen);
	}	
	return res;
}

template <unsigned N>
N_queens<N> random_queens_configuration()
{
	static std::mt19937 re(std::chrono::system_clock::now().time_since_epoch().count());
	return random_queens_configuration<N>(re);
}

/**
 * @brief Generates successors by moving around the queens
 */
template <unsigned N>
struct Move_piece_generator
{
	Move_piece_generator() : re_(std::chrono::system_clock::now().time_since_epoch().count()),
			bernoulli_dist_(0.5),
			int_dist_(0, N-1) {}
	N_queens<N> operator()(const N_queens<N>& state)
	{
		N_queens<N> successor = state;
		auto& queen = successor.queens_[int_dist_(re_)];
		if (queen.first == N-1)
		{
			move_queen_down(successor, queen);
		}
		else if (queen.first == 0)
		{
			move_queen_up(successor, queen);
		}
		else
		{
			if (bernoulli_dist_(re_))
			{
				move_queen_down(successor, queen);
			}
			else
			{
				move_queen_up(successor, queen);
			}
		}
		return successor;
	};
private:
	void move_queen_up(N_queens<N>& state, typename N_queens<N>::Queen& queen)
	{
		state.board_[queen.first][queen.second] = false;
		state.board_[++queen.first][queen.second] = true;
	}
	void move_queen_down(N_queens<N>& state, typename N_queens<N>::Queen& queen)
	{
		state.board_[queen.first][queen.second] = false;
		state.board_[--queen.first][queen.second] = true;
	}

	std::mt19937 re_;
	std::bernoulli_distribution bernoulli_dist_;
	std::uniform_int_distribution<typename N_queens<N>::Size_type> int_dist_;
};

/**
 * @brief Simple energy evaluation functor for n-queens states
 */
template <unsigned N>
struct Energy_evaluation
{
	float operator()(N_queens<N> state) const
	{
		unsigned conflicts = 0;
		unsigned temp = 0;
		typedef typename N_queens<N>::Size_type Size_type;

		for (Size_type i = 0; i < N; ++i, temp = 0)
		{
			for (Size_type j = 0; j < N; ++j)
				if (state[i][j])
					++temp;
			if (temp > 1)
				conflicts += temp-1;
		}

		for (Size_type i = 0; i < N; ++i, temp = 0)
		{
			for (Size_type j = 0; j < N; ++j)
				if (state[j][i])
					++temp;
			if (temp > 1)
				conflicts += temp-1;
		}

		for (Size_type i = 1; i < N; ++i, temp = 0)
		{
			for (Size_type j = 0, k = i; k+1 > 0; ++j, --k)
				if (state[j][k])
					++temp;
			if (temp > 1)
				conflicts += temp-1;
		}
		for (Size_type i = 1; i < N; ++i, temp = 0)
		{
			for (Size_type j = i, k = N-1; j < N; ++j, --k)
				if (state[j][k])
					++temp;
			if (temp > 1)
				conflicts += temp-1;
		}

		for (Size_type i = 0; i < N; ++i, temp = 0)
		{
			for (Size_type j = 0, k = i; k < N; ++j, ++k)
				if (state[j][k])
					++temp;
			if (temp > 1)
				conflicts += temp-1;
		}
		for (Size_type i = 1; i < N-1; ++i, temp = 0)
		{
			for (Size_type j = i, k = 0; j < N; ++j, ++k)
				if (state[j][k])
					++temp;
			if (temp > 1)
				conflicts += temp-1;
		}

		return conflicts;
	}
};

/**
 * @brief Agent applying a random mutation to an N_queens configuration
 */
template <unsigned N>
struct Mutagen
{
	template <typename Random_engine>
    void operator()(N_queens<N>& state, Random_engine& re)
    {
    	auto& queen = state.queens_[dist_(re)];
    	auto previous_val = queen.first;
    	state.board_[queen.first][queen.second] = false;
        do
        {
        	queen.first = dist_(re);
        }
        while (queen.first == previous_val);
    	state.board_[queen.first][queen.second] = true;
    }
private:
    typedef typename N_queens<N>::Size_type Size_type;
    static thread_local std::uniform_int_distribution<Size_type> dist_;
};

template <unsigned N>
thread_local std::uniform_int_distribution<typename N_queens<N>::Size_type> Mutagen<N>::dist_(0, N - 1);

/**
 * @brief Generates a population of states
 */
template <unsigned N>
struct Pop_generator
{
	template <typename Random_engine>
	std::vector<N_queens<N>> operator()(std::size_t n, Random_engine& re) const
	{
		std::vector<N_queens<N>> v;
		v.reserve(n);
		for (std::size_t i = 0; i < n; ++i)
			v.push_back(random_queens_configuration<N>(re));
		return v;
	}
	template <typename Random_engine>
	N_queens<N> operator()(const N_queens<N>& x, const N_queens<N>& y, Random_engine& re)
	{
		N_queens<N> child;
	    auto from_x = dist_(re);
	    for (Size_type i = 0; i < from_x; ++i)
	    {
	        child.add_queen(x.queens_[i]);
	    }
	    for (Size_type i = from_x; i < N; ++i)
	    {
	        child.add_queen(y.queens_[i]);
	    }
	    return child;
	}
private:
	typedef typename N_queens<N>::Size_type Size_type;
    static thread_local std::uniform_int_distribution<Size_type> dist_;
};

template <unsigned N>
thread_local std::uniform_int_distribution<typename N_queens<N>::Size_type> Pop_generator<N>::dist_(0, N - 1);

#endif
