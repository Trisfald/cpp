/**
 * Some tricks that can be done with simple bit manipulation
 */

#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cassert>
#include <limits>
#include <bitset>

void min();
void round_up_power2();
void mask_least_sign_bit();

int main()
{
	min();
	round_up_power2();
	mask_least_sign_bit();
}

void min()
{
	typedef int type;
	constexpr auto size = 100000u;
	constexpr auto trials = 10000u;

	std::vector<type> v1;
	std::vector<type> v2;
	std::vector<type> mins1;
	std::vector<type> mins2;

    v1.reserve(size);
    v2.reserve(size);
    mins1.reserve(size);
    mins2.reserve(size);

    std::mt19937 re(std::chrono::system_clock::now().time_since_epoch().count());
    std::uniform_int_distribution<int> dist(1, size);

    for (unsigned i = 0; i < size; ++i)
    {
        v1[i] = dist(re);
        v2[i] = dist(re);
    }

    std::cout << "Computing the minimum between two integers\n";

    auto start = std::chrono::steady_clock::now();
    for (unsigned k = 0; k < trials; ++k)
    {
		for (unsigned i = 0; i < size; ++i)
		{
			mins1[i] = v1[i] < v2[i] ? v1[i] : v2[i];
		}
    }
    std::cout << "Simple way - time micro: " <<
    		std::chrono::duration_cast<std::chrono::microseconds>
					(std::chrono::steady_clock::now() - start).count() << std::endl;

    start = std::chrono::steady_clock::now();
    for (unsigned k = 0; k < trials; ++k)
    {
		for (unsigned i = 0; i < size; ++i)
		{
			mins2[i] = v2[i] ^ ((v1[i] ^ v2[i]) & -(v1[i] < v2[i]));
		}
    }
    std::cout << "Bithack way - time micro: " <<
    		std::chrono::duration_cast<std::chrono::microseconds>
    				(std::chrono::steady_clock::now() - start).count() << std::endl;

    assert(mins1 == mins2);

    std::cout << "\n\n";
}

void round_up_power2()
{
    std::cout << "Rounding up to the next power of 2\n";
	int n = 340;
    std::cout << n << " becomes ";
	--n;
	for (unsigned i = 0; (1 << i) < std::numeric_limits<int>::digits; ++i)
	{
		n |= n >> (1 << i);
	}
	++n;
	std::cout << n;
	std::cout << "\n\n";
}

void mask_least_sign_bit()
{
    std::cout << "Masking the least significant bit\n";
    int x = 4308;
    std::cout << std::bitset<std::numeric_limits<int>::digits>(x) << "\n";
    auto r = x & (-x);
    std::cout << std::bitset<std::numeric_limits<int>::digits>(r) << "\n";
	std::cout << "\n\n";
}
