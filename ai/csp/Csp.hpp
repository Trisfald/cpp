#ifndef AI_CSP_CSP_HPP_
#define AI_CSP_CSP_HPP_

#include <memory>
#include <utility>
#include <algorithm>
#include <vector>
#include <functional>
#include <unordered_map>
#include <iterator>
#include <stdexcept>

template <typename K, typename T> class Constraint;
template <typename K, typename T> class Variable;
template <typename K, typename T> class Cnft_variable;
template <typename K, typename T> class Conflict;

template <typename K, typename T>
using Variables = std::vector<Variable<K, T>>;
template <typename K, typename T>
using Cnft_variables = std::vector<Cnft_variable<K, T>>;

template <typename K, typename T>
using Assignment = std::unordered_map<K, Variable<K, T>>;
template <typename K, typename T>
using Cnft_assignment = std::unordered_map<K, Cnft_variable<K, T>>;

template <typename K, typename T>
using Constraints = std::vector<std::unique_ptr<const Constraint<K, T>>>;

template <typename K, typename T>
using Constraints_ref = std::vector<std::reference_wrapper<const Constraint<K, T>>>;

template <typename K, typename T> class Unary_constraint;
template <typename K, typename T> class Binary_constraint;

template <typename K, typename T> struct Assignment_dispatcher;
template <typename Assignment> class Concrete_assignment_dispatcher;

namespace detail
{
	/**
	 * Construct an assignment from a set of variables
	 */
    template <template <typename, typename> class A, typename K, typename T>
    A<K, T> assignment_from_variables(const Variables<K, T>& variables);
}

/**
 * @return True if all variables are set to a valid value
 */
template <typename T>
bool is_complete(const T& assignment)
{
    return assignment.cend() == std::find_if(assignment.cbegin(),
            assignment.cend(),
            [](const auto& e) { return !e.second.set(); });
}

/**
 * Base class representing a constraint.
 * Can't be inherited from (use unary or binary constraint).
 */
template <typename K, typename T>
class Constraint
{
	template <typename U, typename Y> friend class Unary_constraint;
	template <typename U, typename Y> friend class Binary_constraint;
	Constraint() = default;
public:
    virtual ~Constraint() = default;
    virtual bool hold(const T&, const Variable<K, T>&, const Assignment_dispatcher<K, T>&) const = 0;
    virtual bool binary() const = 0;
};

/**
 * Base class for unary constraints
 */
template <typename K, typename T>
class Unary_constraint : public Constraint<K, T>
{
public:
	~Unary_constraint() override = default;
	bool binary() const final { return false; }
};

/**
 * Base class for binary constraints
 */
template <typename K, typename T>
class Binary_constraint : public Constraint<K, T>
{
public:
	~Binary_constraint() override = default;
    using Constraint<K, T>::hold;
	virtual bool hold(const T& value_1, const T& value_2) const = 0;
	virtual const K& variable_1_id() const = 0;
	virtual const K& variable_2_id() const = 0;
	bool binary() const final { return true; }
};

template <typename K, typename T>
bool operator==(const Variable<K, T>&, const T&) noexcept;

/**
 * Csp variable, inclusive of its domain of allowed values and current value
 */
template <typename K, typename T>
class Variable
{
	template <typename VK, typename VT>
	friend bool operator==(const Variable<VK, VT>&, const VT&) noexcept;
public:
    typedef K key_type;
    typedef T value_type;
    Variable(const K& id, const std::vector<T>& domain, const Constraints_ref<K, T>& constraints)
    : id_(id), domain_(domain), constraints_(std::make_shared<const Constraints_ref<K, T>>(constraints))
    {}
    bool set() const { return set_; }
    void assign(const T& value)
    {
    	value_ = value;
    	set_ = true;
    }
    void clear() { set_ = false; }
    template <typename Assignment>
    bool consistent(const T& value, const Assignment& assignment) const;
    auto& domain() { return domain_; };
    const auto& domain() const { return domain_; };
    const auto& constraints() const { return *constraints_; }
    const K& id() const { return id_; }
    const T& value() const { return value_; }
private:
    K id_;
    std::vector<T> domain_;
    const std::shared_ptr<const Constraints_ref<K, T>> constraints_;
    bool set_ = false;
    T value_ = T();
};

/**
 * @return The subset of a variable's domain for which each value is consistent with the given assignment
 */
template <typename K, typename T, typename Assignment>
auto get_consistent_domain(const Variable<K, T>& variable, const Assignment& assignment);

template <typename K, typename T>
bool operator==(const Variable<K, T>& lhs, const T& rhs) noexcept
{
	return lhs.set() && lhs.value_ == rhs;
}

template <typename K, typename T>
bool operator!=(const Variable<K, T>& lhs, const T& rhs) noexcept
{
	return !(lhs == rhs);
}

template <typename K, typename T>
class Cnft_variable : public Variable<K, T>
{
public:
    typedef Variable<K, T> Base;
    using Base::key_type;
    using Base::value_type;
    Cnft_variable(const Base& var) : Base(var) {} 
    // TODO can be improved by adding conflicts set logic that could be used to backjump
};

template <typename K, typename T>
bool operator==(const Cnft_variable<K, T>& lhs, const T& rhs) noexcept
{
	return Cnft_variable<K, T>::Base::operator==(rhs);
}

template <typename K, typename T>
bool operator!=(const Cnft_variable<K, T>& lhs, const T& rhs) noexcept
{
	return Cnft_variable<K, T>::Base::operator!=(rhs);
}

/**
 * Class representing the abstraction of a csp problem.
 * It's purpose is only that of storing data for an accurate description of the csp.
 */
template <typename K, typename T>
class Csp
{
public:
	Csp(const Csp&) = delete;
	Csp(Csp&& rhs) noexcept	: variables_(std::move(rhs.variables_)), constraints_(std::move(rhs.constraints_)) {}
	Csp(Variables<K, T>&& variables, Constraints<K, T>&& constraints)
	: variables_(std::move(variables)), constraints_(std::move(constraints))
	{}
	const auto& variables() const { return variables_; }
	const auto& constraints() const { return constraints_; }
private:
    const Variables<K, T> variables_;
    Constraints<K, T> constraints_;
};

template <typename K, typename T>
template <typename Assignment>
bool Variable<K, T>::consistent(const T& value, const Assignment& assignment) const
{
	for (const auto& constraint : *constraints_)
	{
		if (!constraint.get().hold(value, *this, Concrete_assignment_dispatcher<Assignment>(assignment)))
		{
			return false;
		}
	}
	return true;
}

template <typename K, typename T, typename Assignment>
auto get_consistent_domain(const Variable<K, T>& variable, const Assignment& assignment)
{
	std::vector<T> subset;
	std::copy_if(variable.domain().cbegin(),
			variable.domain().cend(),
			std::back_inserter(subset),
			[&variable, &assignment](const auto& e) { return variable.consistent(e, assignment); });
	return subset;
}

template <typename K, typename T>
struct Assignment_dispatcher
{
    virtual const T& get_value(const K& key) const = 0;
    virtual ~Assignment_dispatcher() = default;
};

template <typename Assignment>
class Concrete_assignment_dispatcher : public Assignment_dispatcher<typename Assignment::key_type, 
        typename Assignment::mapped_type::value_type>
{
    typedef typename Assignment::key_type key_type;
    typedef typename Assignment::mapped_type::value_type value_type;
public:
    Concrete_assignment_dispatcher(const Assignment& assignment) : assignment_(assignment) {}
    virtual const value_type& get_value(const key_type& key) const override
    {
        auto it = assignment_.find(key);
        if (it == assignment_.cend())
        {
        	throw std::out_of_range("constraint is pointing to inexistant variable");
        }
        return (*it).second.value();
    }
private:
    const Assignment& assignment_;
};

namespace detail
{
    template <template <typename, typename> class A, typename K, typename T>
    A<K, T> assignment_from_variables(const Variables<K, T>& variables)
    {
        A<K, T> result;
        for (const auto& var : variables)
        {
            result.insert(std::make_pair(var.id(), typename A<K, T>::value_type::second_type(var)));
        }
        return result;
    }
}

#endif
