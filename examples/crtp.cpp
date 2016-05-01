/**
 * Two examples showing the usefulness of crtp
 */

#include <iostream>

template <typename T>
class Base
{
public:
	void method()
	{
		impl().method();
	}
private:
	T& impl()
	{
		return *static_cast<T*>(this);
	}
};

struct Derived_a : Base<Derived_a>
{
	void method()
	{
		std::cout << "derived A method called" << std::endl;
	}
};

struct Derived_b : Base<Derived_b>
{
	void method()
	{
		std::cout << "derived B method called" << std::endl;
	}
};

template <typename T>
void invoke(Base<T>* t)
{
	t->method();
}



template <typename T>
struct Comparison_gifter
{};

template <typename T>
bool operator==(const Comparison_gifter<T>& lhs, const Comparison_gifter<T>& rhs)
{
	const T& lhs_d = static_cast<const T&>(lhs);
	const T& rhs_d = static_cast<const T&>(rhs);
	return !(lhs_d < rhs_d) && !(rhs_d < lhs_d);
}

template <typename T>
bool operator>(const Comparison_gifter<T>& lhs, const Comparison_gifter<T>& rhs)
{
	const T& lhs_d = static_cast<const T&>(lhs);
	const T& rhs_d = static_cast<const T&>(rhs);
	return rhs_d < lhs_d;
}

// Poor struct has only the < operator defined
struct Poor_struct : public Comparison_gifter<Poor_struct>
{
	Poor_struct(int i) : i(i) {}
	int i;
};

bool operator<(const Poor_struct& lhs, const Poor_struct& rhs)
{
	return lhs.i < rhs.i;
}


int main()
{
	// Compile time polymorphism
	Derived_a a;
	Derived_b b;
	invoke(&a);
	invoke(&b);


	// Mixin that adds comparison operators
	Poor_struct ps_1(4);
	Poor_struct ps_2(4);
	std::cout << "Are the two objects equal? " << std::boolalpha << (ps_1 == ps_2) << std::endl;

	return 0;
}
