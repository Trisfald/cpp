/**
 * Used defined stream buffer that prints everything in uppercase to the standard out
 * Based on an example from the book 'The C++ Standard Library'
 */

#include <iostream>
#include <cctype>
#include <streambuf>
#include <unistd.h>
#include <array>
#include <memory>
#include <utility>

class outbuf: public std::streambuf
{
	static constexpr std::streamsize buffer_size_ = 512;
	typedef std::array<char, buffer_size_> Buffer;
public:
	outbuf()
	{
		setp(buffer_->data(), buffer_->data() + (buffer_size_ - 1));
	}
	~outbuf() override
	{
		sync();
	}
protected:
	int flush_buffer()
	{
		int n = pptr()-pbase();
		for (std::streamsize i = 0; i < n; ++i)
		{
			(*buffer_)[i] = toupper((*buffer_)[i]);
		}
		if (write(1, buffer_->data(), n) != n)
		{
			return traits_type::eof();
		}
		pbump (-n);
		return n;
	}
	int_type overflow(int_type c) override
	{
		if (c != traits_type::eof())
		{
			*pptr() = c;
			pbump(1);
		}
		if (flush_buffer() == traits_type::eof())
		{
			return traits_type::eof();
		}
		return c;
	}
	int sync() override
	{
		return (flush_buffer() == traits_type::eof()) ? -1 : 0;
	}
private:
	std::unique_ptr<Buffer> buffer_ = std::make_unique<Buffer>();
};

int main()
{
	// Create special output buffer
	outbuf ob;
	// Initialize output stream with that output buffer
	std::ostream out(&ob);

	out << 42 << ", a string and a character: " << 'c' << std::endl;
}
