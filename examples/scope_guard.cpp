/**
 * Using scope guard constructs to perform certain actions when the scope is abandoned.
 * Needs an extension for std::uncaught_exceptions(). C++17 has it and it also has class template deduction that greatly helps to reduce boilerplate.
 */

#include <iostream>
#include <stdexcept>

template <typename F>
class Scope_guard
{
public:
    Scope_guard(const F& func = F()) : func_(func) {}
    ~Scope_guard() { func_(); }
private:
    F func_;
};

template <typename F>
class Scope_guard_ex
{
public:
    Scope_guard_ex(const F& func = F()) : exception_count_(std::uncaught_exceptions()), func_(func) {}
protected:
    auto exception_count() const { return exception_count_; }
    auto& func() { return func_; }
private:
    int exception_count_;
    F func_;
};

template <typename F>
class Scope_guard_success : private Scope_guard_ex<F>
{
public:
    using Scope_guard_ex<F>::Scope_guard_ex;
    ~Scope_guard_success() { if (std::uncaught_exceptions() == this->exception_count()) this->func()(); }
};

template <typename F>
class Scope_guard_fail : private Scope_guard_ex<F>
{
public:
    using Scope_guard_ex<F>::Scope_guard_ex;
    ~Scope_guard_fail() noexcept { if (std::uncaught_exceptions() > this->exception_count()) this->func()(); }
};


// Helper functions

template <typename F>
auto make_scope_guard(const F& func = F())
{
    return Scope_guard<F>(func);
}

template <typename F>
auto make_scope_guard_success(const F& func = F())
{
    return Scope_guard_success<F>(func);
}

template <typename F>
auto make_scope_guard_fail(const F& func = F())
{
    return Scope_guard_fail<F>(func);
}


int main()
{
    std::cout << "/**" << "\n"
            << " * Using standard scope guard, always executed" << "\n"
            << " */" << "\n";
    {
        std::cout << "enter scope" << "\n";
        int a = 2;
        auto guard = make_scope_guard([] { std::cout << "Scope guard activated" << "\n"; });
        int b = 2 + a;
        std::cout << "operation result = " << b << "\n";
        std::cout << "exit scope" << "\n";
    }
    
    std::cout << "\n/**" << "\n"
            << " * Using scope guard success and fail when no exception is thrown" << "\n"
            << " */" << "\n";    
    {
        std::cout << "enter scope" << "\n";
        auto guard_success = make_scope_guard_success([] { std::cout << "Scope guard success activated" << "\n"; });
        auto guard_fail = make_scope_guard_fail([] { std::cout << "Scope guard fail activated" << "\n"; });
        int a = 2;
        int b = 3 + a;
        std::cout << "operation result = " << b << "\n";
        std::cout << "exit scope" << "\n";
    }
    
    std::cout << "\n/**" << "\n"
            << " * Using scope guard success and fail when an exception is thrown" << "\n"
            << " */" << "\n";    
    try
    {
        std::cout << "enter scope" << "\n";
        auto guard_success = make_scope_guard_success([] { std::cout << "Scope guard success activated" << "\n"; });
        auto guard_fail = make_scope_guard_fail([] { std::cout << "Scope guard fail activated" << "\n"; });
        int a = 2;
        int b = 3 + a;
        throw std::runtime_error("runtime error!");
        std::cout << "operation result = " << b << "\n";
        std::cout << "exit scope" << "\n";
    }
    catch (const std::exception& e) 
    {
        std::cout << "handling exception: " << e.what() << "\n"; 
    }
}
