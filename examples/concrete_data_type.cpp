/**
 * Control an object's scope and lifetime by allowing or disallowing dynamic allocation / automatic storage
 */

#include <cstddef>

class No_automatic_base
{
public:
	virtual ~No_automatic_base() = default;
};

class No_automatic : public No_automatic_base
{
protected:
	~No_automatic() override = default;
};

class No_dynamic
{
private:
    static void* operator new(std::size_t); // disallow new
    static void* operator new(std::size_t, void*);  // disallow placement new
};


int main()
{
	// Scoped variable is not allowed
	// No_automatic o;
	No_automatic_base* po = new No_automatic();
	delete po;

	No_dynamic a;
	(void) a;
	// Dynamic allocation is not allowed
	// auto pa = new No_dynamic();
}
