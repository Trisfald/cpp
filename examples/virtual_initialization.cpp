/**
 * Simulate virtual calls inside a base object's constructor
 */

#include <iostream>

class Base
{
public:
	Base()
	{
		std::cout << "Called base constructor\n";
	}
};

template <typename T>
class Base_initializer : public Base
{
protected:
	Base_initializer()
	{
		T::init();
	}
};

class Derived_1 : public Base_initializer<Derived_1>
{
public:
	Derived_1()
	{
		std::cout << "Derived_1 constructor finishes\n";
	}
	static void init()
	{
		std::cout << "Derived_1 initialized\n";
	}
};

class Derived_2 : public Base_initializer<Derived_2>
{
public:
	Derived_2()
	{
		std::cout << "Derived_2 constructor finishes\n";
	}
	static void init()
	{
		std::cout << "Derived_2 initialized\n";
	}
};

int main()
{
	Derived_1 a;
	std::cout << std::endl;
	Derived_2 b;
}
