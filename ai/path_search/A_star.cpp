/**
	A* search is an informed search algorithm. It expands nodes with minimal f(n) = g(n) + h(n).
	A* is complete and optimal for trees if h(n) is admissible and for graphs if h(n) is consistent.
 */

#include "A_star.hpp"
#include "Parallel_A_star.hpp"
#include <chrono>
#include <utility>
#include "puzzle_board.hpp"

typedef Puzzle_board<3> Puzzle_8;
typedef Puzzle_board<4> Puzzle_15;
template <unsigned N> using Gen = Puzzle_successors_gen<N>;
template <unsigned N> using Heuristic = Puzzle_heuristic_manhattan<N>;

template <typename T> using A_star = A_star_search<T, Puzzle_action, Gen<T::size>, Heuristic<T::size>, Full_result>;
template <typename T> using IDA_star = IDA_star_search<T, Puzzle_action, Gen<T::size>, Heuristic<T::size>, Full_result>;
template <typename T> using IEA_star = IEA_star_search<T, Puzzle_action, Gen<T::size>, Heuristic<T::size>, Full_result>;
template <typename T> using BA_star = BA_star_search<T, Puzzle_action, Gen<T::size>, Heuristic<T::size>, Full_result>;

template <typename Solver, typename... Args>
void run_puzzle(Solver& solver, Args&&... args);

// Start and goal for 8-puzzle
Puzzle_8 start_8({{{{8, 6, 7}}, {{2, 5, 4}}, {{3, 0, 1}}}});
Puzzle_8 goal_8({{{{1, 2, 3}}, {{4, 5, 6}}, {{7, 8, 0}}}});
// Start and goal for 15-puzzle
Puzzle_15 start_15({{{{7, 6, 4, 5}}, {{12, 3, 2, 1}}, {{15, 14, 8, 0}}, {{11, 10, 9, 13}}}});
Puzzle_15 goal_15({{{{1, 2, 3, 4}}, {{5, 6, 7, 8}}, {{9, 10, 11, 12}}, {{13, 14, 15, 0}}}});

int main()
{
	// A* solvers
	A_star<Puzzle_8> a_star_8(Gen<Puzzle_8::size>{}, Heuristic<Puzzle_8::size>{goal_8});
	A_star<Puzzle_15> a_star_15(Gen<Puzzle_15::size>{}, Heuristic<Puzzle_15::size>{goal_15}); // very high memory usage!
	// IDA* solvers
	IDA_star<Puzzle_8> ida_star_8(Gen<Puzzle_8::size>{}, Heuristic<Puzzle_8::size>{goal_8});
	IDA_star<Puzzle_15> ida_star_15(Gen<Puzzle_15::size>{}, Heuristic<Puzzle_15::size>{goal_15});  // very very slow!
	// IEA* solvers
	IEA_star<Puzzle_8> iea_star_8(Gen<Puzzle_8::size>{}, Heuristic<Puzzle_8::size>{goal_8});
	IEA_star<Puzzle_15> iea_star_15(Gen<Puzzle_15::size>{}, Heuristic<Puzzle_15::size>{goal_15}); // takes 1.5h on my machine
	// BA* solvers
	BA_star<Puzzle_8> ba_star_8(Gen<Puzzle_8::size>{}, Heuristic<Puzzle_8::size>{goal_8}); // fastest on multi-core processors but not optimal
	BA_star<Puzzle_15> ba_star_15(Gen<Puzzle_15::size>{}, Heuristic<Puzzle_15::size>{goal_15}); // fastest on multi-core processors but not optimal

	run_puzzle(a_star_8, start_8, goal_8, 50);
	//run_puzzle(a_star_15, start_15, goal_15, 100);
	//run_puzzle(ida_star_8, start_8, goal_8, 50);
	//run_puzzle(ida_star_15, start_15, goal_15, 100);
	//run_puzzle(iea_star_8, start_8, goal_8, 50);
	//run_puzzle(iea_star_15, start_15, goal_15, 100);
	//run_puzzle(ba_star_8, start_8, goal_8, 50);
	//run_puzzle(ba_star_15, start_15, goal_15, 100);
}

template <typename Solver, typename... Args>
void run_puzzle(Solver& solver, Args&&... args)
{
	auto start = std::chrono::steady_clock::now();
	auto result = solver(std::forward<Args>(args)...);
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start);

	if (result.second == Solver::Result::failure)
	{
		std::cout << "Failure" << std::endl;
	}
	else if (result.second == Solver::Result::cutoff)
	{
		std::cout << "Cutoff" << std::endl;
	}
	else
	{
		std::cout << "Result:" << std::endl;
		for (const auto& i : *result.first)
		{
			std::cout << i.state << "Action: " << i.action << std::endl << std::endl;
		}
		std::cout << "Number of steps: " << result.first->size() << std::endl;
		std::cout << "Indicative time of execution: " << duration.count() << " microseconds" << std::endl;
	}
}

