/**
 * Store a callable with a fixed signature in a type erasing class
 */

#include <iostream>
#include <memory>
#include <utility>

// Implementation

struct Base_impl
{
    virtual ~Base_impl() = default;
    virtual void operator()(int) = 0;
};

template <typename F>
struct Impl : Base_impl
{
    Impl(const F& f = F()) : func(f) {}
    
    void operator()(int i) override
    {
        func(i);
    }
    
    F func;
};

struct Function
{
    template <typename F>
    Function(const F& f = F())
    {
        ptr = std::make_unique<Impl<F>>(f);
    }
    
    void operator()(int i)
    {
        ptr->operator()(i);
    }
    
    std::unique_ptr<Base_impl> ptr;
};


// Test functions

void print_value(int i)
{
    std::cout << i << std::endl;
}

struct Print_double_value
{
    void operator()(int i)
    {
        std::cout << i*2 << std::endl;
    }
};


int main()
{
    Function f(&print_value);
    f(5);
    
    Function fx2(Print_double_value{});
    fx2(5);
    
    Function fx3([](int i) { std::cout << i*3 << std::endl; });
    fx3(5);
}
