#include <future>
#include <chrono>
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <functional>
#include "Ring_buffer.hpp"

struct X
{
	char data[1000000];
	int i = 0;
	X() = default;
	X(int n) : i(n) {}
};
bool operator<(const X& lhs, const X& rhs) { return lhs.i < rhs.i; }
bool operator==(const X& lhs, const X& rhs) { return lhs.i == rhs.i; }

std::atomic<bool> flag{false};

template <typename B, typename V>
int th1(B& buffer, V& vector)
{
	int i = 0;
	while (!flag)
	{
		buffer.push(i);
        vector.push_back(i);
        ++i;
	}
	return i;
}

template <typename B, typename V>
int th2(B& buffer, V& vector)
{
	int j = 0;
	while (!flag || !buffer.empty())
	{
        if (buffer.empty())
        {
            continue;
        }
        auto item = buffer.pop();
        vector.push_back(item);
        ++j;
	}
	return j;
}

template <typename T, unsigned N>
void test(std::chrono::milliseconds time)
{
	auto buffer = std::make_unique<Threadsafe_ring_buffer<T, N>>();
	std::vector<T> vi, vj;

	auto f1 = std::async(std::launch::async,
			th1<Threadsafe_ring_buffer<T, N>, std::vector<T>>,
			std::ref(*buffer),
			std::ref(vi));
	auto f2 = std::async(std::launch::async,
			th2<Threadsafe_ring_buffer<T, N>, std::vector<T>>,
			std::ref(*buffer),
			std::ref(vj));

	std::this_thread::sleep_for(time);
	flag.store(true);

	auto i = f1.get();
	auto j = f2.get();
	flag.store(false);

	std::cout << "written: " << i << std::endl;
    std::cout << "read: " << j << std::endl;
    std::cout << "equal content: " << std::boolalpha << (vi == vj) << std::endl;

    std::vector<T> common;
    std::set_intersection(vi.cbegin(), vi.cend(), vj.cbegin(), vj.cend(), std::back_inserter(common));
    std::cout << "number of equal elements: " << common.size() << std::endl;
}

int main()
{
	using namespace std::chrono_literals;

	// Test with a small buffer
	test<int, 10>(100ms);
	std::cout << "\n\n";

	// Test with a large buffer
	test<int, 10000>(100ms);
	std::cout << "\n\n";

	// Test with a large struct
	test<X, 10>(100ms);
}
