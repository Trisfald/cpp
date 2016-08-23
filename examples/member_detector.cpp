/**
 * Allows to check if a given class has a member with a certain name.
 * Caveat: the member's name is hardcoded
 */

#include <type_traits>
#include <iostream>
#include <iomanip>
                        
namespace detail
{
    
    template <typename T>                                                              
    class Has_member_foo                                                        
    {                                                                                 
    private:                                                                          
        using Yes = char[2];                                                          
        using No = char[1];                                                          
                                                                                    
        struct Fallback { int foo; }; // note: must have a member named exactly as the searched-for member                                        
        struct Derived : T, Fallback {};                                             
                                                                                    
        template <typename U>                                                          
        static No& test(decltype(U::foo)*); // note: must take a pointer to a member named exactly as the searched-for member                                        
        template <typename U>                                                       
        static Yes& test(U*);                                                      
                                                                                    
    public:                                                                           
        static constexpr bool value = sizeof(test<Derived>(nullptr)) == sizeof(Yes); 
    };   

}
                                                                                  
template <typename T>                                                              
struct Has_member_foo : public std::integral_constant<bool, detail::Has_member_foo<T>::value>            
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

int main()
{
    std::cout << std::boolalpha << 
            "Does A have a member foo? " << Has_member_foo<A>::value << "\n" <<
            "Does B have a member foo? " << Has_member_foo<B>::value << "\n" <<
            "Does C have a member foo? " << Has_member_foo<C>::value << "\n";
}
