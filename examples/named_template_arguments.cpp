/**
 * Technique for using named template arguments to selectively override defaults
 */

#include <iostream>

// Core implementation
template <typename... Args>
struct Argument_selector : Args...
{};

template <typename T, typename D>
struct Argument_1_holder : virtual D
{
    typedef T Argument_1;
};

template <typename T, typename D>
struct Argument_2_holder : virtual D
{
    typedef T Argument_2;
};

template <typename T, typename D>
struct Argument_3_holder : virtual D
{
    typedef T Argument_3;
};

template <typename T, typename D>
struct Argument_4_holder : virtual D
{
    typedef T Argument_4;
};


// Define some sample policies
struct Policy_1
{
    void operator()() { std::cout << "Policy 1 called" << std::endl; }
};

struct Policy_2
{
    void operator()() { std::cout << "Policy 2 called" << std::endl; }
};

struct Policy_3
{
    void operator()() { std::cout << "Policy 3 called" << std::endl; }
};

struct Policy_custom
{
    void operator()() { std::cout << "Policy custom called" << std::endl; }
};


// Customized implementation depending on the target class/policies
struct Defaults
{
   typedef Policy_1 Argument_1;
   typedef Policy_2 Argument_2;
   typedef Policy_3 Argument_3;
};

template <typename Arg_1 = Policy_1,
        typename Arg_2 = Policy_2,
        typename Arg_3 = Policy_3>
class My_class
{
    typedef Argument_selector<Arg_1, Arg_2, Arg_3> Selector;
    typedef typename Selector::Argument_1 Argument_1;
    typedef typename Selector::Argument_2 Argument_2;
    typedef typename Selector::Argument_3 Argument_3;
public:
    void operator()()
    {
        Argument_1{}();
        Argument_2{}();
        Argument_3{}();
    }
};
    

int main()
{
    My_class<Argument_2_holder<Policy_custom, Defaults>>{}();
}
