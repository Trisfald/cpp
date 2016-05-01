/**
 * How to specialize only a template's member function
 */

#include <iostream>

template <typename T>
struct X
{
	void std_member() const
	{
		std::cout << "standard member called" << std::endl;
	}
	void foo() const
	{
		std::cout << "base version of foo called" << std::endl;
	}
};

template<>
void X<int>::foo() const
{
	std::cout << "specialized version of foo called" << std::endl;
}

int main()
{
	X<float> f;
	X<int> i;
	f.foo();
	i.foo();
}
