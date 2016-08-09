/**
 * Example of expression template used to reduce the number of temporaries created during an expression evaluation
 */

#include <vector>
#include <iostream>
#include <cstddef>
#include <utility>

// Basic vector expression to be used with crtp
template <typename E>
struct Vector_expr
{
	std::size_t size() const { return static_cast<const E*>(this)->size(); }
	decltype(auto) operator[](std::size_t i) { return (*static_cast<E*>(this))[i]; }
	decltype(auto) operator[](std::size_t i) const { return (*static_cast<const E*>(this))[i]; }
	operator E&() const { return *static_cast<E*>(this); }
	operator const E&() const { return *static_cast<const E*>(this); }
};

template <typename T>
class Vector : public Vector_expr<Vector<T>>
{
	std::vector<T> data_;
public:
	typedef T value_type;
	Vector(std::initializer_list<T> li) : data_(li) {}
    template <typename E>
    Vector(const Vector_expr<E>& v) // A vector can be instantiated from an expression
	{
    	data_.reserve(v.size());
        for (size_t i = 0; i != v.size(); ++i)
        {
            data_.push_back(v[i]); // Triggers the expression's evaluation
        }
    }
    std::size_t size() const { return data_.size(); }
	decltype(auto) operator[](std::size_t i) { return data_[i]; }
	decltype(auto) operator[](std::size_t i) const { return data_[i]; }
};

template <typename L, typename R>
class Vector_sum : public Vector_expr<Vector_sum<L, R>>
{
	const Vector_expr<L>& l_;
	const Vector_expr<R>& r_;
public:
	typedef typename L::value_type value_type;
	Vector_sum(const Vector_expr<L>& lhs, const Vector_expr<R>& rhs) : l_(lhs), r_(rhs)
	{}
	value_type operator[](std::size_t i) const { return l_[i] + r_[i]; } // Here the real sum is done
	std::size_t size() const { return l_.size(); }
};

template <typename L, typename R>
Vector_sum<L, R> operator+(const Vector_expr<L>& lhs, const Vector_expr<R>& rhs)
{
	return Vector_sum<L, R>(lhs, rhs);
}

int main()
{
	Vector<int> v1{1, 2, 3, 4, 5};
	Vector<int> v2{4, 3, 1, 6, 0};
	Vector<int> v3{2, 2, 2, 1, 2};

	Vector<int> v4 = v1 + v2 + v3;
	for (std::size_t i = 0; i < v4.size(); ++i)
	{
		std::cout << v4[i] << " ";
	}
}
