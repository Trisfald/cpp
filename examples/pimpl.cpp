#include "Pimpl.hpp"
// All heavy dependencies go here
#include <string>
#include <vector>
#include <list>
#include <map>
#include <iostream>
 
struct MyClass::MyClass_impl
{
	std::string s = "Hello!";
	std::vector<long> v;
	std::list<float> l;
	std::map<double, std::string> map;
};

std::ostream& operator<<(std::ostream& os, const MyClass& item)
{
	os << item.pimpl_->s;
	return os;
}

MyClass::MyClass() : pimpl_(std::make_unique<MyClass_impl>()) {}

MyClass::~MyClass() {}
