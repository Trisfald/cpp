/**
 * How to make an exact replica of one policy but instantiated with different type parameters
 */

#include <iostream>
#include <cstddef>

template <typename T>
class Policy
{
public:
	template <typename U>
	struct rebind // this allows the rebinding of this policy with another type
	{
		typedef Policy<U> other;
	};
	auto size() const { return sizeof(T); }
};

template <typename T, typename Policy = Policy<T>>
class My_class : public Policy
{
public:
	typedef typename Policy::template rebind<T>::other Clone_policy; // we can now typedef a differently parameterized clone_policy
	Clone_policy clone_policy_instance_;
};

int main()
{
	My_class<long long, Policy<char>> o;
	std::cout << o.size() << std::endl;
	std::cout << o.clone_policy_instance_.size() << std::endl;
}
