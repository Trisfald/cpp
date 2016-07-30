/**
 * How to pack several variable-bits length unsigned numbers together
 */

#include <iostream>
#include <vector>
#include <limits>
#include <cassert>
#include <memory>

class Bit_packet
{
	typedef unsigned char Byte;
public:
	template <typename T>
	unsigned append(T t)
	{
		auto v = t;
		unsigned significant_bits = 1;
		while (v >>= 1)
		{
			++significant_bits;
		}
		t = t << (bits - (significant_bits % bits));
		auto arr = std::make_unique<Byte[]>((significant_bits / bits) + 1);
		for (unsigned i = 0; i < (significant_bits / bits) + 1; ++i)
		{
			arr[((significant_bits / bits)) - i] = t >> (i * bits);
		}
		append(arr.get(), significant_bits);
		return significant_bits;
	}
	void append(Byte* p, unsigned n)
	{
		for (unsigned i = 0; i < n; i++)
		{
			v_.push_back(*p & (1 << (bits-1 - i%bits)));
			if ((i+1) % bits == 0)
			{
				++p;
			}
		}
	}
	template <typename T>
	T get(unsigned start, unsigned n) const
	{
		assert(n <= sizeof(T)*bits);
		auto arr = std::make_unique<Byte[]>((n / bits) + 1);
		get(start, n, arr.get());
		T t = 0;
		for (unsigned i = 0; i < (n / bits); ++i)
		{
			t += arr[i] << ((n % bits) + bits * (n / bits - i - 1));
		}
		t += arr[n / bits] >> (bits - n % bits);
		return t;
	}
	void get(unsigned start, unsigned n, Byte* dst) const
	{
		*dst = 0;
		for (unsigned i = 0; i < n; i++)
		{
			*dst |= static_cast<Byte>(v_[start+i]) << (bits-1 - i%bits);
			if ((i+1) % bits == 0)
			{
				*++dst = 0;
			}
		}
	}
	std::vector<bool>::size_type size() const { return v_.size(); }
private:
	constexpr static auto bits = std::numeric_limits<Byte>::digits;
	std::vector<bool> v_;
};


int main()
{
	unsigned i = 15, k = 501, j = 30;
	Bit_packet bp;

	std::cout << "appending.. " << i << "\n";
	auto size_i = bp.append(i);
	std::cout << "appending.. " << k << "\n";
	auto size_k = bp.append(k);
	std::cout << "appending.. " << j << "\n";
	auto size_j = bp.append(j);

	std::cout << "size: " << bp.size() << "\n";

	unsigned counter = 0;
	std::cout << "retrieving first number: " << bp.get<unsigned>(counter, size_i) << "\n";
	counter += size_i;
	std::cout << "retrieving second number: " << bp.get<unsigned>(counter, size_k) << "\n";
	counter += size_k;
	std::cout << "retrieving third number: " << bp.get<unsigned>(counter, size_j) << "\n";
}
