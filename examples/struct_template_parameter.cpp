/**
 * How to pass a struct of values as template argument
 */

#include <iostream>

struct Args_t { int x, y, z; };

constexpr Args_t my_args { 1, 2, 3 };

template <Args_t const& Args>
struct Foo
{
    void operator()()
    {
    	std::cout << "arg x: " << Args.x << ", arg y: " << Args.y << ", arg z: " << Args.z << std::endl;
    }
};

int main()
{
    Foo<my_args> f;
    f();
}
