/**
 * Policy classes
 */

#include <iostream>
#include <string>

template <typename Output_policy, typename Language_policy>
class Hello : public Output_policy, public Language_policy // public inheritance allows clients to extend the class interface
{
	// Method 1: put names in scope
    // using Output_policy::print;
    // using Language_policy::message;
public:
    void run() const
    {
    	// Method 2: use scoped access
    	Output_policy::print(Language_policy::message());
    }
};

class Write_to_cout
{
protected:
	~Write_to_cout() = default; // a protected destructor ensures that a pointer-to-policy can't be deleted
    template <typename Message_type>
    void print(const Message_type& message) const
    {
        std::cout << message << std::endl;
    }
};

class English_policy
{
protected:
	~English_policy() = default;
    std::string message() const
    {
        return "Hello!";
    }
};

class Italian_policy
{
protected:
	~Italian_policy() = default;
    std::string message() const
    {
        return "Ciao!";
    }
};


int main()
{
	Hello<Write_to_cout, English_policy> english;
	english.run();

	Hello<Write_to_cout, Italian_policy> italian;
	italian.run();
}
