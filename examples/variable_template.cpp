/**
 * Introduction to variable templates,  defines a family of variables or static data members. 
 */

#include <cassert>

// A constant variable template
template <typename T> constexpr T constexpr_var = T(3);
template <> constexpr auto constexpr_var<char> = 5; // specialization is allowed

// A normal variable template
template <typename T> T var = T(3);

// We can use variable templates to make compile time computations
template <unsigned N> constexpr auto factorial { N * factorial<N - 1> };
template <> constexpr auto factorial<0> = 1;

// Combine constexpr variable templates with constexpr functions
template <unsigned N, unsigned M>
constexpr auto factorial_sum()
{
    return factorial<N> + factorial<M>;
}

int main()
{
    static_assert(constexpr_var<int> == 3, "variable has the wrong value");
    static_assert(constexpr_var<float> == 3, "variable has the wrong value");
    static_assert(constexpr_var<char> == 5, "variable has the wrong value");
    
    var<int> = -3; // change var value but only for int
    assert(var<int> == -3);
    assert(var<float> == 3);
    
    static_assert(factorial<4> == 24, "wrong factorial result");
    
    static_assert(factorial_sum<4, 3>() == 30, "wrong factorial result");
}
