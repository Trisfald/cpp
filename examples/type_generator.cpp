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

int main()
{
	Dictionary<std::string>::type str_dictionary; // a map of <string, string> with custom alloc
	Dictionary<double>::type d_dictionary; // a map of <string, double> with custom alloc
	str_dictionary.insert({{"H", "house"}});
	d_dictionary.insert({{"green", 0.8}});
}
