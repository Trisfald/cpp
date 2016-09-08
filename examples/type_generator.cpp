/**
 * Add a level of abstraction to ease the creation of new types
 */

#include <functional>
#include <string>
#include <unordered_map>
#include <utility>

template <typename T>
struct My_alloc : std::allocator<T>
{};

template <typename T>
struct Dictionary
{
	typedef std::string Key;
	typedef T Value;
	typedef My_alloc<std::pair<const Key, T>> Alloc;
	typedef std::unordered_map<Key, T, std::hash<Key>, std::equal_to<Key>, Alloc> type;
};

template <typename T>
using Dictionary_cpp11 = std::unordered_map<std::string,
		T,
		std::hash<std::string>,
		std::equal_to<std::string>,
		My_alloc<std::pair<const std::string, T>>>;

int main()
{
	Dictionary<std::string>::type str_dictionary; // a map of <string, string> with custom alloc
	Dictionary<double>::type d_dictionary; // a map of <string, double> with custom alloc
	Dictionary_cpp11<int> i_dictionary; // a map of <string, int> with custom alloc, c++11 alias templates
	str_dictionary.insert({{"H", "house"}});
	d_dictionary.insert({{"green", 0.8}});
	i_dictionary.insert({{"Rome", -5}});
}
