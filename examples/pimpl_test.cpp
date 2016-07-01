/**
 * The pimpl idiom is used to hide implementation details and reduce dependencies
 */

#include "Pimpl.hpp"
#include <iostream>

int main()
{
	MyClass o;
	std::cout << o << std::endl;
}
