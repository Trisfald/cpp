/**
 * Ways to go from tuples to parameter packs and viceversa
 */

#include <tuple>
#include <iostream>
#include <utility>

template <typename... Args>
struct Tupler
{
	typedef std::tuple<Args...> Tuple;
	Tupler() = default;
	Tupler(Args&&... args) : tuple(std::make_tuple<Args...>(std::forward<Args>(args)...)) {}
	Tuple tuple;
};

void foople(std::tuple<int, char, float> t)
{
	std::cout << "called foople(tuple) with: " << std::get<0>(t) << ", " << std::get<1>(t) << ", " << std::get<2>(t)
			<< std::endl;
}

void from_pack_to_tuple()
{
	Tupler<int, char, float> tpl;
	foople(tpl.tuple);
	Tupler<int, char, float> tpl_2(3, 'a', 2.5);
	foople(tpl_2.tuple);
}



template <typename... Args>
void foopack()
{
	std::cout << "called foopack<Args...>()" << std::endl;
}

struct Foopack
{
	template <typename... Args>
	void operator()(Args&&...)
	{
		std::cout << "called Foopack<Args...>()(args)" << std::endl;
	}
};

void foo(float, unsigned)
{
	std::cout << "called foo(float, unsigned)" << std::endl;
}

template <typename... Args>
struct Pack
{};

template <typename... Ts>
struct Storer
{
    using Args = Pack<Ts...>;
};

template<typename... Args>
void foostore(Pack<Args...>)
{
	foopack<Args...>();
}

template <typename Storer>
void foostore(Storer p)
{
	foostore(typename Storer::Args());
}

template <typename Function, typename Tuple, size_t... I>
auto call(Function f, Tuple t, std::index_sequence<I ...>)
{
     return f(std::get<I>(t)...);
}

template <typename Function, typename Tuple>
auto call(Function f, Tuple t)
{
    static constexpr auto size = std::tuple_size<Tuple>::value;
    return call(f, t, std::make_index_sequence<size>{});
}

void from_tuple_to_pack()
{
	foostore(Storer<float, unsigned>{}); // This's a simple parameter pack storing without tuple

	std::tuple<float, unsigned> tpl;
	std::tuple<float, unsigned> tpl_2(3.4, 49);
	call(foo, tpl_2);
	call(Foopack{}, tpl_2);
}

int main()
{
	from_pack_to_tuple();
	std::cout << "\n";
	from_tuple_to_pack();
}
