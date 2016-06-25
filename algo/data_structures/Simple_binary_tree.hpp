#ifndef ALGO_DATA_STRUCTURES_SIMPLE_BINARY_TREE_HPP_
#define ALGO_DATA_STRUCTURES_SIMPLE_BINARY_TREE_HPP_

#include <stdexcept>
#include <cstddef>
#include <iterator>
#include <type_traits>
#include <utility>

template <typename T, typename UT = std::remove_cv_t<T>> class B_tree_iterator;
template <typename T> class Simple_binary_tree;

template <typename T>
typename Simple_binary_tree<T>::Node_ptr min_node(typename Simple_binary_tree<T>::Node_ptr start);
template <typename T>
typename Simple_binary_tree<T>::Node_ptr max_node(typename Simple_binary_tree<T>::Node_ptr start);
template <typename T, typename F>
void traverse_post(typename Simple_binary_tree<T>::Node_ptr node_ptr, const F& func);
template <typename T>
void rebase_in_parent(typename Simple_binary_tree<T>::Node_ptr ptr, typename Simple_binary_tree<T>::Node_ptr new_ptr);

/**
 * @brief Binary tree offering strong exception safety guarantee
 * Requirements:
 * - T destructor must not throw
 */
template <typename T>
class Simple_binary_tree
{
    struct Node;
	template <typename Y, typename UY>
	friend class B_tree_iterator; 
public:
    typedef Node* Node_ptr;
    typedef T element_type;
	typedef B_tree_iterator<T> iterator;
	typedef B_tree_iterator<const T> const_iterator;

    Simple_binary_tree() = default;
    ~Simple_binary_tree();

    void insert(const T& value);
    bool find(const T& value) const;
    bool erase(const T& value);
    T minimum() const;
    T maximum() const;
    iterator begin() { return iterator(min_node<T>(root)); }
    iterator end() { return iterator(); }
private:
    void insert(const T& value, Node_ptr& leaf, Node_ptr parent);
    Node_ptr find(const T& value, const Node_ptr& node) const;

    Node_ptr root = nullptr;
};

template <typename T>
struct Simple_binary_tree<T>::Node
{
    Node(const T& value, Node_ptr parent) : value(value), parent(parent) {}
    T value;
    Node_ptr parent;
    Node_ptr left = nullptr;
    Node_ptr right = nullptr;
};

template <typename T>
Simple_binary_tree<T>::~Simple_binary_tree()
{
    traverse_post<T>(root, [] (auto p) { delete p; } );
}

template <typename T>
bool Simple_binary_tree<T>::find(const T& value) const
{
    return find(value, root) != nullptr;
}

template <typename T>
typename Simple_binary_tree<T>::Node_ptr Simple_binary_tree<T>::find(const T& value, const Node_ptr& node_ptr) const
{
    if (node_ptr == nullptr)
    {
        return nullptr;
    }
    if (node_ptr->value == value)
    {
        return node_ptr;
    }
    if (value < node_ptr->value)
    {
        return find(value, node_ptr->left);
    }
    else
    {
        return find(value, node_ptr->right);
    }
}

template <typename T>
T Simple_binary_tree<T>::minimum() const
{
	if (root == nullptr)
    {
		throw std::out_of_range("tree is empty!");
	}
	return min_node<T>(root)->value;
}

template <typename T>
T Simple_binary_tree<T>::maximum() const
{
	if (root == nullptr)
    {
		throw std::out_of_range("tree is empty!");
	}
	return max_node<T>(root)->value;
}

template <typename T>
void Simple_binary_tree<T>::insert(const T& value)
{
    if (root == nullptr)
    {
        auto ptr = new Node(value, nullptr);
        root = ptr;
    }
    else
    {
        insert(value, value < root->value ? root->left : root->right, root);
    }
}

template <typename T>
void Simple_binary_tree<T>::insert(const T& value, Node_ptr& leaf, Node_ptr parent)
{
    if (leaf == nullptr)
    {
        auto ptr = new Node(value, parent);
        leaf = ptr;
    }
    else if (value < leaf->value)
    {
        insert(value, leaf->left, leaf);
    }
    else
    {
        insert(value, leaf->right, leaf);
    }
}

template <typename T>
bool Simple_binary_tree<T>::erase(const T& value)
{
    auto ptr = find(value, root);
    if (ptr == nullptr)
    {
        return false;
    }
    if (ptr->left != nullptr)
    {
        if (ptr->right != nullptr)
        {
			auto rightmost_min = min_node<T>(ptr->right);
			// Move the rightmost min in place of ptr
			rebase_in_parent<T>(ptr, rightmost_min);
			// Connect the ptr children to their new parent and viceversa
			if (rightmost_min != ptr->right)
			{
				rightmost_min->parent->left = nullptr;
				rightmost_min->parent = ptr->parent;
				rightmost_min->left = ptr->left;
				rightmost_min->right = ptr->right;
			}
			else
			{
				rightmost_min->left = ptr->left;
				rightmost_min->parent = ptr->parent;
			}
			if (rightmost_min->left != nullptr)
			{
				rightmost_min->left->parent = rightmost_min;
			}			
			if (rightmost_min->right != nullptr)
			{
				rightmost_min->right->parent = rightmost_min;		
			}
			delete ptr;
        }
        else
        {
			rebase_in_parent<T>(ptr, ptr->left);
			ptr->left->parent = ptr->parent;
			delete ptr;
        }
    }
    else
    {
        if (ptr->right != nullptr)
        {
			rebase_in_parent<T>(ptr, ptr->right);
			ptr->right->parent = ptr->parent;
			delete ptr;
        }
        else
        {
			rebase_in_parent<T>(ptr, nullptr);
            delete ptr;			
        }
    }
    return true;
}

template <typename T, typename F>
void traverse_post(typename Simple_binary_tree<T>::Node_ptr node_ptr, const F& func)
{
	if (node_ptr == nullptr)
	{
		return;
	}
	traverse_post<T>(node_ptr->left, func);
	traverse_post<T>(node_ptr->right, func);
	func(node_ptr);
}

template <typename T>
typename Simple_binary_tree<T>::Node_ptr min_node(typename Simple_binary_tree<T>::Node_ptr start)
{
	while (start->left != nullptr)
    {
        start = start->left;
    }
    if (start != nullptr)
    {
        return start;
    }
    return nullptr;
}

template <typename T>
typename Simple_binary_tree<T>::Node_ptr max_node(typename Simple_binary_tree<T>::Node_ptr start)
{
	while (start->right != nullptr)
    {
        start = start->right;
    }
    if (start != nullptr)
    {
        return start;
    }
    return nullptr;
}

template <typename T>
void rebase_in_parent(typename Simple_binary_tree<T>::Node_ptr ptr, typename Simple_binary_tree<T>::Node_ptr new_ptr)
{
	if (ptr->parent == nullptr)
	{
		return;
	}
	if (ptr == ptr->parent->left)
	{
		ptr->parent->left = new_ptr;
	}
	else
	{
		ptr->parent->right = new_ptr;
	}
}

template <typename T, typename UT>
class B_tree_iterator : public std::iterator<std::forward_iterator_tag, UT, std::ptrdiff_t, T*, T&>
{
	template <typename Y> friend class Simple_binary_tree;
	typedef typename Simple_binary_tree<UT>::Node* Itr;
    explicit B_tree_iterator(Itr itr) : itr(itr) {}	
public:
	B_tree_iterator() : itr(nullptr) {}

	void swap(B_tree_iterator& other) noexcept
    {
        using std::swap;
        swap(itr, other.itr);
    }
    operator B_tree_iterator<const T>() const
    {
        return B_tree_iterator<const T>(itr);
    }
    T& operator* () const
    {
        return itr->value;
    }
    T& operator-> () const
    {
        return itr->value;
    }	
	B_tree_iterator& operator++()
	{
	    if (itr == nullptr)
		{
			return *this;
		}
		if (itr->right != nullptr)
		{
			itr = min_node<T>(itr->right);
		}
		else
		{
			do
			{
				auto itr_old = itr;
				itr = itr->parent;
				if (itr == nullptr)
				{
					break;
				}
				if (itr_old == itr->left)
				{
					break;
				}
			}
			while (1);
		}
		return *this;
	}
	B_tree_iterator& operator++(int)
	{
		auto tmp(*this);
		++(*this);
		return tmp;
	}
    template <typename Y>
    bool operator==(const B_tree_iterator<Y>& rhs) const
    {
        return itr == rhs.itr;
    }
    template<class Y>
    bool operator!=(const B_tree_iterator<Y>& rhs) const
    {
        return itr != rhs.itr;
    }	
private:
	Itr itr;
};

#endif
