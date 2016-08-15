/**
 * Allows a class template to partecipate in the same implicit type conversions (coercion) as its parameterizing type.
 */

class Base {};
class Derived : public Base {};

template <typename T>
class Helper
{
public:
	Helper(T* ptr) : ptr_(ptr) {}
	Helper(const Helper&) = default; // remember that copy/move constructor will be implicitly declared even if there's a templated one
	Helper(Helper&&) = delete;
	Helper& operator=(const Helper&) = default; // same goes for the assignment operator
	Helper& operator=(Helper&&) = delete;

    template <class U>
    Helper(const Helper<U>& rhs) : ptr_(rhs.ptr())
	{}
    template <class U>
    Helper& operator=(const Helper<U>& rhs)
    {
      ptr_ = rhs.ptr();
      return *this;
    }

    T* ptr() const { return ptr_; }
private:
	T* ptr_;
};

int main()
{
	Base b;
	Derived d;
	Helper<Base> helper_b(&b);
	Helper<Derived> helper_d(&d);
	helper_b = helper_d;
}
