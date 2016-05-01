/**
 * Taking advantage of iterator traits in order to use different implementations for different iterators
 */

#include <iostream>
#include <iterator>
#include <vector>
#include <list>

template <typename Random_Iterator>
void foo(Random_Iterator it, std::random_access_iterator_tag)
{
	std::cout << "random iterator access" << std::endl;
}

template <typename Input_Iterator>
void foo(Input_Iterator it, std::input_iterator_tag)
{
	std::cout << "input iterator access" << std::endl;
}

template <typename Iterator>
void foo(Iterator it)
{
	typename std::iterator_traits<Iterator>::iterator_category cat;
	foo(it, cat);
}

int main()
{
	std::vector<int> v1;
	std::list<int> l1;
	foo(v1.end());
	foo(l1.end());
}

