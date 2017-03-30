/**
 * Example showing how do fold expressions work
 * Requires C++17
 */

#include <string>
#include <iostream>

// A struct for tracing operator+ calls

struct X
{
    X(const std::string& val) : val(val) {}
    std::string val;
};

std::ostream& operator<<(std::ostream& os, const X& x)
{
    os << x.val;
    return os;
}

X operator+(const X& rhs, const X& lhs)
{
    return X("(" + rhs.val + "_" + lhs.val + ")");
}


// Helper functions to perform folding

template <typename... Args>
auto right_fold(Args... args) 
{ 
    const auto result = (args + ...);
    return result;
}

template <typename... Args>
auto left_fold(Args... args) 
{ 
    const auto result = (... + args);
    return result;
}

template <typename... Args>
auto right_binary_fold(Args... args) 
{ 
    const auto result = (args + ... + (X("t")));
    return result;
}

template <typename... Args>
auto left_binary_fold(Args... args) 
{ 
    const auto result = ((X("t")) + ... + args);
    return result;
}

int main()
{
    X a("a"), b("b"), c("c");
    std::cout << "Right folding a, b and c: " << right_fold(a, b, c) << "\n";
    std::cout << "Left folding a, b and c: " << left_fold(a, b, c) << "\n";
    std::cout << "Right binary folding a, b and c with t: " << right_binary_fold(a, b, c) << "\n";
    std::cout << "Left binary folding a, b and c with t: " << left_binary_fold(a, b, c) << "\n";
}
