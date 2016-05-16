#include "N_queens.hpp"
#include "GA.hpp"
#include <iostream>
#include <ios>

template <typename Solver, typename... Args> void run(Solver& solver, Args&&... args);

int main()
{
	static constexpr unsigned size = 8;
	GA<N_queens<size>, Pop_generator<size>, Energy_evaluation<size>, Mutagen<size>> ga;
	PGA<N_queens<size>, Pop_generator<size>, Energy_evaluation<size>, Mutagen<size>> pga;
	std::cout << "--- Standard version\n";
	run(ga, 1000, 1000);
	std::cout << "\n\n--- Parallel version\n";
	run(pga, 1000, 1000);
}

template <typename Solver, typename... Args>
void run(Solver& solver, Args&&... args)
{
	auto start = std::chrono::steady_clock::now();
	auto result = solver(std::forward<Args>(args)...);
	auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - start);
	std::cout << "Is goal? " << std::boolalpha << result.goal() << "\n";
	std::cout << result << "\n";
	std::cout << "Fitness: " << Energy_evaluation<8>{}(result) << "\n";
	std::cout << "Indicative time of execution: " << duration.count() << " microseconds" << "\n";
}
