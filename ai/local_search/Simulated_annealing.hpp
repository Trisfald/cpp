#ifndef AI_SEARCHING_SIMULATED_ANNEALING_HPP_
#define AI_SEARCHING_SIMULATED_ANNEALING_HPP_

#include <limits>
#include <chrono>
#include <utility>
#include <random>
#include <cmath>

/**
 * @brief Generic simulated annealing algorithm
 *
 * @tparam State represents the states of the problem
 * @tparam Generator callable returning a random successor of a state
 * @tparam Energy callable returning the estimated energy of a state (the lower the better)
 * @tparam Schedule callable mapping time (intended as number of iterations) to temperature
 */
template <typename State, typename Generator, typename Energy, typename Schedule>
class Simulated_annealing
{
public:
	typedef State State_type;
	Simulated_annealing(const Generator& generator = Generator(),
		const Energy& energy = Energy(),
		const Schedule& schedule = Schedule()) :
			generator_(generator), energy_(energy), schedule_(schedule) 
	{}
    State operator()(State start);
private:
    Generator generator_;
    Energy energy_;
    Schedule schedule_;
};

namespace detail
{

    template <typename State>
    inline void update_current(State& current, float& current_e, const State& next, float next_e)
    {
        current = std::move(next);
        current_e = next_e;
    }

    inline bool random_check(float p)
    {
    	static thread_local std::mt19937 re(std::chrono::system_clock::now().time_since_epoch().count());
        static thread_local std::uniform_real_distribution<float> dist(0, 1);
        return p < dist(re);
    }

}

template <typename State, typename Generator, typename Energy, typename Schedule>
State Simulated_annealing<State, Generator, Energy, Schedule>::operator()(State start)
{
    State current = std::move(start);
    float current_e = energy_(current);
    State best;
    float best_e = std::numeric_limits<float>::max();
    for (unsigned long long t = 1; t < std::numeric_limits<decltype(t)>::max(); ++t)
    {
        const float temp = schedule_(t);
        if (temp == 0)
		{
            return best;
		}
        const auto next = generator_(current);
        const float next_e = energy_(next);
        const float delta_e = current_e - next_e;
        if (delta_e > 0)
        {
        	if (next_e < best_e)
        	{
        		best = next;
        		best_e = next_e;
        	}
            detail::update_current(current, current_e, next, next_e);
        }
        else if (detail::random_check(std::exp(delta_e / temp)))
        {
        	detail::update_current(current, current_e, next, next_e);
        }
    }
    return best;
}

#endif
