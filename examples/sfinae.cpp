/**
 * Use sfinae to resolve a function call at compile time depending on a certain condition
 */

#include <iostream>
#include <type_traits>

template <typename T>
std::enable_if_t<std::is_integral<T>::value> foo(T t)
{
    std::cout << "integral call" << std::endl;
}

template <typename T>
std::enable_if_t<std::is_floating_point<T>::value> foo(T t)
{
    std::cout << "floating point call" << std::endl;
}

template <typename T>
struct Check1
{
    template <typename U = T>
    U read(typename std::enable_if_t<std::is_same<U, int>::value>* = nullptr)
    {
        std::cout << "Check 1: type is int" << std::endl;
        return 1;
    }

    template <typename U = T>
    U read(typename std::enable_if_t<std::is_same<U, double>::value>* = nullptr)
    {
        std::cout << "Check 1: type is double" << std::endl;
        return 1.0;
    }
};

// Second best
template <typename T>
struct Check2
{
    template <typename U = T, typename std::enable_if_t<std::is_same<U, int>::value>* = nullptr>
    U read()
    {
        std::cout << "Check 2: type is int" << std::endl;
        return 1;
    }

    template <typename U = T, typename std::enable_if_t<std::is_same<U, double>::value>* = nullptr>
    U read()
    {
        std::cout << "Check 2: type is double" << std::endl;
        return 1.0;
    }
};

template <typename T>
struct Check3
{
    template <typename U = T>
    typename std::enable_if_t<std::is_same<U, int>::value, U> read()
    {
        std::cout << "Check 3: type is int" << std::endl;
        return 1;
    }

    template <typename U = T>
    typename std::enable_if_t<std::is_same<U, double>::value, U> read()
    {
        std::cout << "Check 3: type is double" << std::endl;
        return 1.0;
    }
};

// Preferred way to go
template <typename T>
struct Check4
{
    template <typename U = T, std::enable_if_t<std::is_same<U, int>::value, int> = 0>
    U read()
    {
        std::cout << "Check 4: type is int" << std::endl;
        return 1;
    }


    template <typename U = T, std::enable_if_t<std::is_same<U, double>::value, int> = 0>
    U read()
    {
        std::cout << "Check 4: type is double" << std::endl;
        return 1.0;
    }    
};

int main()
{
    foo(1);
    foo(2.0);

    Check1<int> check1_int;
    check1_int.read();
    Check1<double> check1_double;
    check1_double.read();

    Check2<int> check2_int;
    check2_int.read();
    Check2<double> check2_double;
    check2_double.read();

    Check3<int> check3_int;
    check3_int.read();
    Check3<double> check3_double;
    check3_double.read();

    Check4<int> check4_int;
    check4_int.read();
    Check4<double> check4_double;
    check4_double.read();

    return 0;
}
