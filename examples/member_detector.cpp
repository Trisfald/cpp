/**
 * Allows to check if a given class has a member with a certain name.
 * Alternate version checks for a member function with a precise signature.
 * Caveat: the member's name is hardcoded
 */

#include <type_traits>
#include <iostream>
#include <iomanip>
                        
namespace detail
{
    
    template <typename T>                                                              
    struct Has_member_foo
    {                                                                                 
    private:                                                                          
        using Yes = char[2];                                                          
        using No = char[1];                                                          
                                                                                    
        struct Fallback { int foo; }; // note: must have a member named exactly as the searched-for member                                        
        struct Derived : T, Fallback {};                                             
                                                                                    
        template <typename U>                                                          
        static No& test(decltype(U::foo)*); // note: must take a pointer to a member named exactly as the searched-for member                                        
        template <typename U>                                                       
        static Yes& test(...);
                                                                                    
    public:                                                                           
        static constexpr bool value = sizeof(test<Derived>(nullptr)) == sizeof(Yes); 
    };   


    template <typename T>
    struct Has_member_fn_foo
    {
    private:
        using Yes = char[2];
        using No = char[1];

        template <typename U, int (U::*)(int)> struct Impl {}; // here we specify the exact signature wanted
        template <typename U> static Yes& test(Impl<U, &U::foo>*); // here we specify the member's name
        template <typename U> static No& test(...);

    public:
        static constexpr bool value = sizeof(test<T>(nullptr)) == sizeof(Yes);
    };

}


// Integral constant wrappers for convenience
template <typename T>                                                              
struct Has_member_foo : public std::integral_constant<bool, detail::Has_member_foo<T>::value>
{};

template <typename T>
struct Has_member_fn_foo : public std::integral_constant<bool, detail::Has_member_fn_foo<T>::value>
{};


// Example structs
struct A
{
    int foo;
};

struct B
{
    int foo(int);
};

struct C
{};

struct D
{
	int foo(char);
};

int main()
{
    std::cout << std::boolalpha << 
            "Does A have a member foo? " << Has_member_foo<A>::value << "\n" <<
            "Does B have a member foo? " << Has_member_foo<B>::value << "\n" <<
            "Does C have a member foo? " << Has_member_foo<C>::value << "\n" <<
			"Does D have a member foo? " << Has_member_foo<D>::value << "\n";

    std::cout << std::boolalpha <<
            "Does A have a member function int foo(int)? " << Has_member_fn_foo<A>::value << "\n" <<
			"Does B have a member function int foo(int)? " << Has_member_fn_foo<B>::value << "\n" <<
			"Does C have a member function int foo(int)? " << Has_member_fn_foo<C>::value << "\n" <<
			"Does D have a member function int foo(int)? " << Has_member_fn_foo<D>::value << "\n";
}
