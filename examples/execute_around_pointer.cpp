/**
 * Provide a smart pointer object that transparently executes actions before and after each function call on an object.
 * A special form of aspect oriented programming.
 */

#include <iostream>
#include <vector>
#include <memory>

template <typename Next, typename T>
class Aspect
{
protected:
	Aspect (T t): t_(t) {}
	T  t_;
public:
	Next operator->() {	return Next(t_);	}
};

template <typename Aspect, typename T>
class Aspect_weaver
{
public:
	Aspect_weaver (T t): t_(t) {}
    Aspect operator->() { return Aspect(t_); }
private:
	T  t_;
};

template <typename Next, typename T>
struct Concrete_aspect_1 : Aspect<Next, T>
{
	Concrete_aspect_1(T t) : Aspect<Next, T>(t)
	{
		std::cout << "Before first aspect. Size = " << this->t_->size() << std::endl;
	}
	~Concrete_aspect_1()
	{
		std::cout << "After first aspect. Size = " << this->t_->size() << std::endl;
	}
};

template <typename Next, typename T>
struct Concrete_aspect_2 : Aspect<Next, T>
{
	Concrete_aspect_2(T t) : Aspect<Next, T>(t)
	{
		std::cout << "Before second aspect. Capacity = " << this->t_->capacity() << std::endl;
	}
	~Concrete_aspect_2()
	{
		std::cout << "After second aspect. Capacity = " << this->t_->capacity() << std::endl;
	}
};

int main()
{
	typedef std::shared_ptr<std::vector<int>> vec;
	Aspect_weaver <Concrete_aspect_2<Concrete_aspect_1<vec, vec>, vec>, vec> a(std::make_shared<vec::element_type>());
	a->push_back(1);
}
