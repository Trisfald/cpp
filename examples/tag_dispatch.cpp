/**
 * Compile time function call resolution using a condition and template specializations
 */

#include <type_traits>
#include <iostream>

// There must be a forward declaration before specializations can be defined
template <typename T, bool> struct X;

template <typename T>
struct X<T, true>
{
	static void method(T t)
	{
		std::cout << "called on pod" << std::endl;
	}
};

template <typename T>
struct X<T, false>
{
	static void method(T t)
	{
		std::cout << "called on non pod" << std::endl;
	}
};

template <typename T>
void invoke(T t)
{
	X<T, std::is_pod<T>::value>::method(t);
}

struct Pod
{
	int i;
};

struct No_pod
{
	No_pod(int i) : i(i) {}
	int i;
};

int main()
{
	Pod x;
	No_pod y(0);
	invoke(x);
	invoke(y);
	return 0;
}
