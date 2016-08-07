#include <cassert>
#include "vEB_tree.hpp"

/**
 *  Van Emde Boas tree
 */

int main()
{
	// Tree with items: 2^2^K where K = 4
	vEB_tree<4> tree;
	assert(!tree.contains(400));

	tree.insert(10);
	tree.insert(400);
	tree.insert(811);
	tree.insert(203);
	assert(tree.contains(811));
	assert(tree.successor(10) == 203);

	tree.remove(203);
	assert(!tree.contains(203));
	assert(tree.successor(10) == 400);
}
