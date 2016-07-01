#include <iosfwd>
#include <memory>

class MyClass;
std::ostream& operator<<(std::ostream&, const MyClass&);

class MyClass
{
	friend std::ostream& operator<<(std::ostream&, const MyClass&);
public:
	MyClass();
	~MyClass();
private:
	struct MyClass_impl;
	std::unique_ptr<MyClass_impl> pimpl_;
};
