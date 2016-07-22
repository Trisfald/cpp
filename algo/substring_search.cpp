/**
 * Demonstration on various methods for searching a substring inside a string
 */

#include <string>
#include <cstddef>
#include <random>
#include <chrono>
#include <iostream>
#include <algorithm>
#include <type_traits>
#include <utility>
#include <vector>

void test(const std::string& str, const std::string& substr);
std::string build_rnd_text(std::size_t l, const std::string& substring);
std::string build_similar_text(std::size_t l, const std::string& substring);
bool find_simple(const std::string& str, const std::string& substr);
bool find_hashing(const std::string& str, const std::string& substr);
bool find_stdlib(const std::string& str, const std::string& substr);
bool find_z_algo(const std::string& str, const std::string& substr);
template <typename Duration, typename Fn, typename... Arg>
std::pair<std::result_of_t<Fn(Arg...)>, typename Duration::rep> benchmark(Fn, Arg&&...);

template <typename T>
T my_pow(T x, T p)
{
  if (p == 0) return 1;
  if (p == 1) return x;

  T tmp = pow(x, p / 2);
  return (p % 2 == 0) ? tmp * tmp : x * tmp * tmp;
}

class Rolling_hash
{
public:
	Rolling_hash(const std::string&);
	void next(char erase, char append);
	std::size_t operator()() { return n_; }
private:
	std::size_t len_;
	std::size_t n_ = 0;
	static constexpr std::size_t base_ = 25;
};

int main()
{
	std::cout << "Search in a random text:\n\n";
	std::string find_str("abcdz");
	auto rnd_text = build_rnd_text(100000, find_str);
	test(rnd_text, find_str);

	std::cout << "\n\nSearch in a text very uniform:\n\n";
	std::string find_str_2("aaaaaaaaaaaaaaz");
	auto similar_text = build_similar_text(100000, find_str_2);
	test(similar_text, find_str_2);
}

void test(const std::string& str, const std::string& substr)
{
	{
		auto result = benchmark<std::chrono::microseconds>(find_simple, str, substr);
		std::cout << "Simple way: found = " << std::boolalpha << result.first << ", microseconds: " << result.second
				<< "\n";
	}
	{
		auto result = benchmark<std::chrono::microseconds>(find_hashing, str, substr);
		std::cout << "With hashing: found = " << std::boolalpha << result.first << ", microseconds: " << result.second
				<< "\n";
	}
	{
		auto result = benchmark<std::chrono::microseconds>(find_z_algo, str, substr);
		std::cout << "Z algorithm: found = " << std::boolalpha << result.first << ", microseconds: " << result.second
				<< "\n";
	}
	{
		auto result = benchmark<std::chrono::microseconds>(find_stdlib, str, substr);
		std::cout << "Stl library: found = " << std::boolalpha << result.first << ", microseconds: " << result.second
				<< "\n";
	}
}

std::string build_rnd_text(std::size_t l, const std::string& substring)
{
	static std::mt19937 re(std::chrono::system_clock::now().time_since_epoch().count());
	static std::uniform_int_distribution<char> dist('a', 'y');
	std::string text;
	text.reserve(l);
	if (l <= substring.size())
	{
		text = substring;
	}
	else
	{
		for (std::size_t i = 0; i <= l - substring.size(); ++i)
		{
			if (i == ((l - substring.size()) / 2))
			{
				text += substring;
			}
			else
			{
				text.push_back(dist(re));
			}
		}
	}
	return text;
}

std::string build_similar_text(std::size_t l, const std::string& substring)
{
	std::string text;
	text.reserve(l);
	for (std::size_t i = 0; i <= l - substring.size(); ++i)
	{
		if (i == ((l - substring.size()) / 2))
		{
			text += substring;
		}
		else
		{
			if (i % 20 == 0)
			{
				text.push_back('b');
			}
			else
			{
				text.push_back('a');
			}
		}
	}
	return text;
}

bool find_simple(const std::string& str, const std::string& substr)
{
	const auto substr_len = substr.size();
	for (auto it = str.cbegin(); it != (str.cend() - substr_len + 1); ++it)
	{
		if (std::equal(it, it + substr_len, substr.cbegin()))
		{
			return true;
		}
	}
	return false;
}

bool find_hashing(const std::string& str, const std::string& substr)
{
	if (str.size() < substr.size())
	{
		return false;
	}
	const auto substr_len = substr.size();
	Rolling_hash substr_hash(substr);
	Rolling_hash hash(str.substr(0, substr_len));
	for (auto it = str.cbegin(); it != (str.cend() - substr_len + 1); ++it)
	{
		if (it != str.cbegin())
		{
			hash.next(*(it - 1), *(it + substr_len - 1));
		}
		if (hash() == substr_hash())
		{
			if (std::equal(it, it + substr_len, substr.cbegin()))
			{
				return true;
			}
		}
	}
	return false;
}

template <typename Duration, typename Fn, typename... Arg>
std::pair<std::result_of_t<Fn(Arg...)>, typename Duration::rep> benchmark(Fn fn, Arg&&... args)
{
	auto start = std::chrono::steady_clock::now();
	auto result = fn(std::forward<Arg>(args)...);
	auto duration = std::chrono::duration_cast<Duration>(std::chrono::steady_clock::now() - start);
	return {result, duration.count()};
}

Rolling_hash::Rolling_hash(const std::string& s) : len_(s.size())
{
	for (std::size_t i = 0; i < len_; ++i)
	{
		n_ += static_cast<std::size_t>(s[i]) * my_pow(base_, (len_ - i - 1));
	}
}

void Rolling_hash::next(char erase, char append)
{
	n_ = base_ * (n_ - static_cast<std::size_t>(erase) * my_pow(base_, len_ - 1)) + static_cast<std::size_t>(append);
}

bool find_stdlib(const std::string& str, const std::string& substr)
{
	return std::string::npos != str.find(substr);
}

bool find_z_algo(const std::string& str, const std::string& substr)
{
    std::string concat = substr + "&" + str;
    const auto concat_len = concat.size();
    const auto substr_len = substr.size();
    std::vector<std::size_t> z(concat_len, 0);

    // Build the z-vector
    std::size_t l = 0, r = 0, k;
	for (std::size_t i = 1; i < concat_len; ++i)
	{
		if (i > r)
		{
			l = r = i;
			while (r < concat_len && concat[r - l] == str[r])
			{
				++r;
			}
			z[i] = r - l;
			--r;
		}
		else
		{
			k = i - l;
			if (z[k] < r - i + 1)
			{
				z[i] = z[k];
			}
			else
			{
				l = i;
				while (r < concat_len && concat[r - l] == str[r])
				{
					++r;
				}
				z[i] = r - l;
				--r;
			}
		}
	}

    // Search for the substring
    for (std::size_t i = 0; i < concat_len; ++i)
    {
        if (z[i] == substr_len)
        {
        	return true;
        }
    }
	return false;
}
