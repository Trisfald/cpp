#include "N_queens.hpp"
#include "Simulated_annealing.hpp"
#include <iostream>	
	
struct Simple_schedule
{
	float operator()(unsigned long long t) const
	{
		unsigned long long l = 100 - (t / 1000);
		return static_cast<float>(l) / 100.0f;
	}
};

int main()
{
	Simulated_annealing<N_queens<8>, Move_piece_generator<8>, Energy_evaluation<8>, Simple_schedule> sa;
	// It might need a few attempts to find a goal state!
	auto result = sa(random_queens_configuration<8>());
	std::cout << "Is goal? " << std::boolalpha << result.goal() << std::endl;
	std::cout << result << std::endl;
	std::cout << "Energy: " << Energy_evaluation<8>{}(result) << std::endl;
}
