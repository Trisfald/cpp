/**
 * Example of using a trait class to convey information about types
 */

#include <iostream>
#include <string>

struct short_range_tag {};
struct long_range_tag {};
struct speech_tag {};

template <typename T>
struct weapon_traits
{
	typedef typename T::weapon_category weapon_category;
};

template <>
struct weapon_traits<std::string>
{
	typedef speech_tag weapon_category;
};


class Sword
{
public:
	typedef short_range_tag weapon_category;
private:
	char some_var_ = 0;
};

class Bow
{
public:
	typedef long_range_tag weapon_category;
private:
	char some_other_var_ = 0;
};


template <typename T>
void use(const T& item, short_range_tag)
{
	std::cout << "Used a short range weapon!" << std::endl;
}

template <typename T>
void use(const T& item, long_range_tag)
{
	std::cout << "Used a long range weapon!" << std::endl;
}

template <typename T>
void use(const T& item, speech_tag)
{
	std::cout << "The pen is mightier than the sword!" << std::endl;
}

template <typename T>
void use(const T& item)
{
	use(item, typename weapon_traits<T>::weapon_category());
}


int main()
{
	Sword sword;
	Bow bow;
	use(sword);
	use(bow);
	using namespace std::literals::string_literals;
	use("Thou shalt fall"s);
}
