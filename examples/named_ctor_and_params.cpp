/**
 * Example of named constructor to enhance code clarity.
 * Use of named parameters to make a function call more flexible/clear.
 */

#include <string>
#include <iostream>

// Named constructor
class Product
{
public:
	static Product create_with_name(const std::string& name)
	{
		return Product(name, "ZZOO");
	}
	static Product create_with_code(const std::string& code)
	{
		return Product("nameless", code);
	}
private:
	Product(const std::string& name, const std::string& code) : name_(name), code_(code) {} // note: private
	std::string name_;
	std::string code_;
};


// Named parameters
struct Pack
{
	int x = 0;
	int y = 0;
	int theta = 0;
    Pack& set_x(int n)
    {
    	x = n;
    	return *this;
    }
    Pack& set_y(int n)
    {
    	y = n;
    	return *this;
    }
    Pack& set_theta(int n)
    {
    	theta = n;
    	return *this;
    }
    static Pack create() { return Pack(); }
};

void foo(Pack pack)
{
	std::cout << "x: " << pack.x << ", y: " << pack.y << ", theta: " << pack.theta << std::endl;
}


int main()
{
	Product named_one = Product::create_with_name("blue-surfer");
	Product formal_one = Product::create_with_code("BA91");

	foo(Pack::create().set_x(40).set_theta(1));
}
