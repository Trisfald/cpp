/**
 * How to write testable static asserts.
 * Technique developed by Roland Bock.
 */

#include <type_traits>

// Types to indicate error and not error
template <typename T>
struct error : std::false_type {};

struct ok {};


// Wrapper for static assert typedef of our check
struct assert_arg_integral
{
	template <typename T = void>
	assert_arg_integral()
	{
		static_assert(error<T>::value, "argument must be an integral type");
	}
};

template <typename T>
using check_arg = std::conditional_t<std::is_integral<T>::value, ok, assert_arg_integral>;


// Two tag dispatched impl versions
template <typename T>
auto times_two_impl(ok, T t)
{
	return t * 2;
}

template <typename Check, typename T>
Check times_two_impl(Check, T);


// Our checked function
template <typename T>
auto times_two(T t) -> decltype(times_two_impl(check_arg<T>{}, t))
{
	return times_two_impl(check_arg<T>{}, t);
}

int main()
{
	static_assert(std::is_same<decltype(times_two(4)), int>::value, "");
	static_assert(std::is_same<decltype(times_two(2.2)), assert_arg_integral>::value, "");
}
