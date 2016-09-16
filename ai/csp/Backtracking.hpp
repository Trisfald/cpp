#ifndef AI_CSP_BACKTRACKING_HPP_
#define AI_CSP_BACKTRACKING_HPP_

#include "Csp.hpp"
#include <memory>
#include <utility>
#include <algorithm>
#include <vector>
#include <functional>
#include <queue>
#include <iterator>

/**
 * Policy to decide the variables' order of evaluation: first the ones with the smallest domain
 */
class MRV_select_policy
{
protected:
    ~MRV_select_policy() = default;
    template <typename K, typename T>
    Cnft_variable<K, T>& next(Cnft_assignment<K, T>&) const;
};

/**
 * Policy to decide the values' order of evaluation: same order as specified in the domain
 */
class First_order_policy
{
protected:
	~First_order_policy() = default;
    template <typename K, typename T>
	std::vector<T> list(const Cnft_variable<K, T>&, const Cnft_assignment<K, T>&) const;
};

/**
 * Least constraining value
 */
class LCV_order_policy
{
protected:
	~LCV_order_policy() = default;
    template <typename K, typename T>
	std::vector<T> list(Cnft_variable<K, T>&, const Cnft_assignment<K, T>&) const;
};

/**
 * Ensures arc-consistency
 */
class AC3_consistency
{
public:
	/**
	 * @return False if an inconsistency is found, otherwise true
	 */
    template <typename K, typename T>
	bool revise(const Cnft_variable<K, T>&, Cnft_assignment<K, T>&) const;
protected:
    ~AC3_consistency() = default;
private:
    /**
     * @return True if any change occurred
     */
    template <typename K, typename T>
    bool revise(const Binary_constraint<K, T>&, Cnft_assignment<K, T>&) const;
};

template <typename Inference_policy = AC3_consistency,
		typename Select_var_policy = MRV_select_policy,
		typename Value_order_policy = First_order_policy>
class Backtracking_solver : protected Inference_policy,
        public Select_var_policy,
        public Value_order_policy
{
    template <typename K, typename T>
    using Assignment_ptr = std::unique_ptr<Cnft_assignment<K, T>>;
public:
    template <typename K, typename T>
    Assignment_ptr<K, T> operator()(const Csp<K, T>& csp) const
    {
        Cnft_assignment<K, T> start_assignment = detail::assignment_from_variables<Cnft_assignment>(csp.variables());
        return backtrack(start_assignment, csp);
    }
private:
    template <typename K, typename T>
    Assignment_ptr<K, T> backtrack(Cnft_assignment<K, T>& assignment, const Csp<K, T>& csp) const;
};

template <typename Inference_policy, typename Select_var_policy, typename Value_order_policy>
template <typename K, typename T>
Backtracking_solver<Inference_policy, Select_var_policy, Value_order_policy>::Assignment_ptr<K, T>
Backtracking_solver<Inference_policy, Select_var_policy, Value_order_policy>::backtrack(
		Cnft_assignment<K, T>& assignment,
		const Csp<K, T>& csp) const
{
    if (is_complete(assignment))
    {
        return std::make_unique<Cnft_assignment<K, T>>(std::move(assignment));
    }

    auto* variable = &Select_var_policy::next(assignment);

    for (const auto& value : Value_order_policy::list(*variable, assignment))
    {
		const auto old_assignment = assignment;
		variable->assign(value);

		if (Inference_policy::revise(*variable, assignment))
		{
			auto result = backtrack(assignment, csp);
			if (result != nullptr)
			{
				return result;
			}
		}
        assignment = old_assignment;
    }
    return nullptr;
}

template <typename K, typename T>
Cnft_variable<K, T>& MRV_select_policy::next(Cnft_assignment<K, T>& assignment) const
{
	auto it = std::min_element(assignment.begin(),
			assignment.end(),
			[](const auto& e1, const auto& e2)
			{
				if (e1.second.set())
				{
					return false;
				}
				if (e2.second.set())
				{
					return true;
				}
				return e1.second.domain().size() < e2.second.domain().size();
			});
	return (*it).second;
}

template <typename K, typename T>
std::vector<T> LCV_order_policy::list(Cnft_variable<K, T>& variable,
		const Cnft_assignment<K, T>& assignment) const
{
	std::vector<std::pair<unsigned, T>> values_scores;
	for (const auto& value : get_consistent_domain(variable, assignment))
	{
		variable.assign(value);
		unsigned satisfiable_constraints = 0;
		for (const auto& other_var_pair : assignment)
		{
			if (other_var_pair.first == variable.id())
			{
				continue;
			}
			const auto& other_var = other_var_pair.second;
			for (const auto& constraint_ref : other_var.constraints())
			{
				if (constraint_ref.get().binary())
				{
					const auto& constraint = dynamic_cast<const Binary_constraint<K, T>&>(constraint_ref.get());
					if (constraint.variable_2_id() != variable.id())
					{
						continue;
					}
					// Check how many values in other_var's domain can cohesist with the current assignment of variable
					satisfiable_constraints += std::count_if(other_var.domain().cbegin(),
							other_var.domain().cend(),
							[&other_var, &constraint, &value](const auto& other_val)
									{ return constraint.hold(other_val, value); });
				}
			}
		}
		values_scores.push_back(std::make_pair(satisfiable_constraints, value));
		variable.clear();
	}
	std::sort(values_scores.begin(),
			values_scores.end(),
			[](const auto& lhs, const auto& rhs) { return lhs.first > rhs.first; });
	std::vector<T> result;
	result.reserve(values_scores.size());
	std::transform(values_scores.cbegin(),
			values_scores.cend(),
			std::back_inserter(result),
            [](const auto& e) { return e.second; });
	return result;
}

template <typename K, typename T>
std::vector<T> First_order_policy::list(const Cnft_variable<K, T>& variable,
		const Cnft_assignment<K, T>& assignment) const
{
	return get_consistent_domain(variable, assignment);
}

template <typename K, typename T>
bool AC3_consistency::revise(const Cnft_variable<K, T>& variable, Cnft_assignment<K, T>& assignment) const
{
	typedef std::reference_wrapper<const Binary_constraint<K, T>> Arc;
	std::queue<Arc> queue;
	for (const auto& constraint : variable.constraints())
	{
		if (constraint.get().binary())
		{
			queue.push(std::cref(dynamic_cast<const Binary_constraint<K, T>&>(constraint.get())));
		}
	}
	while (!queue.empty())
	{
		auto arc = queue.front();
		queue.pop();
		if (revise(arc.get(), assignment))
		{
			if (variable.domain().empty())
			{
				return false;
			}
			for (const auto& constraint : variable.constraints())
			{
				if (&constraint.get() != &arc.get())
				{
					queue.push(std::cref(dynamic_cast<const Binary_constraint<K, T>&>(constraint.get())));
				}
			}
		}
	}
	return true;
}

template <typename K, typename T>
bool AC3_consistency::revise(const Binary_constraint<K, T>& constraint, Cnft_assignment<K, T>& assignment) const
{
	bool revised = false;

	auto it = assignment.find(constraint.variable_1_id());
	if (it == assignment.cend())
	{
		throw std::out_of_range("constraint is pointing to inexistant variable");
	}
	auto& var_1 = *it;
	it = assignment.find(constraint.variable_2_id());
	if (it == assignment.cend())
	{
		throw std::out_of_range("constraint is pointing to inexistant variable");
	}
	auto& var_2 = *it;

	for (auto it = var_1.second.domain().cbegin(); it != var_1.second.domain().cend(); ++it)
	{
		auto value_it = std::find_if(var_2.second.domain().cbegin(),
				var_2.second.domain().cend(),
				[it, &constraint](const auto& value) { return constraint.hold(*it, value); });
		if (value_it == var_2.second.domain().cend())
		{
			it = var_1.second.domain().erase(it, it + 1);
			revised = true;
			if (it == var_1.second.domain().cend())
			{
				break;
			}
		}
	}
	return revised;
}

#endif
