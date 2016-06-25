#include "Simple_binary_tree.hpp"

#include <iostream>
#include <cassert>
#include <stdexcept>
#include <algorithm>
#include <iterator>

namespace {

	struct X
	{
		X(int value) : value(value) {}
		X(int value, bool throw_e) : value(value)
		{
			if (throw_e)
			{
				throw std::runtime_error("bad constructor!");
			}
		}
		int value;
	};

	bool operator==(X lhs, X rhs) noexcept
	{
		return lhs.value == rhs.value;
	}

	bool operator<(X lhs, X rhs) noexcept
	{
		return lhs.value < rhs.value;
	}

	std::ostream& operator<<(std::ostream& os, X x)
	{
		os << x.value;
		return os;
	}

}

int main()
{
	Simple_binary_tree<X> tree;

	tree.insert(X(6));
	tree.insert(X(3));
	tree.insert(X(9));
	tree.insert(X(2));
	tree.insert(X(4));
	tree.insert(X(7));

	assert(!tree.find(X(30)));
	assert(tree.find(X(2)));

	assert(tree.erase(X(3)));
	assert(!tree.find(X(3)));

	tree.insert(X(10));
	tree.insert(X(2));
	tree.insert(X(1));

	assert(tree.maximum() == X(10));
	assert(tree.minimum() == X(1));

	// 1 2 2 4 6 7 9 10
	std::copy(tree.begin(), tree.end(), std::ostream_iterator<X>(std::cout, " "));
	
	// fail an insert
	try
	{
		tree.insert(X(2, true));
	}
	catch (...)
	{}
	
	// 1 2 2 4 6 7 9 10
	std::cout << "\n";
	std::copy(tree.begin(), tree.end(), std::ostream_iterator<X>(std::cout, " "));
}
