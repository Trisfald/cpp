/**
 * Fast fourier transform
 */

#include <vector>
#include <complex>
#include <valarray>
#include <iostream>
#include <cstddef>
#include <iterator>

typedef std::vector<std::complex<double>> Return_type;
typedef std::valarray<std::complex<double>> Array_type;

void ifft(Array_type&);
void fft(Array_type&);
void pad_to_po2(Array_type&);

int main()
{
	Array_type input = {1.0, 1.0, 1.0, 1.0, 0.0, 1.0};
	// Number of points provided must be a power of two
	pad_to_po2(input);

	std::cout << "Input:\n";
	std::copy(begin(input), end(input), std::ostream_iterator<Array_type::value_type>(std::cout,"\n"));

	std::cout << "\nFFT:\n";
	fft(input);
	std::copy(begin(input), end(input), std::ostream_iterator<Array_type::value_type>(std::cout,"\n"));

	std::cout << "\nIFFT:\n";
	ifft(input);
	std::copy(begin(input), end(input), std::ostream_iterator<Array_type::value_type>(std::cout,"\n"));
}

void ifft(Array_type& arr)
{
	arr = arr.apply(std::conj);
	fft(arr);
	arr = arr.apply(std::conj);
	arr /= arr.size();
}

void fft(Array_type& arr)
{
	static const double PI = 3.141592653589793;
	const auto size = arr.size();
	if (size <= 1)
	{
		return;
	}

	Array_type even = arr[std::slice(0, size / 2, 2)];
	Array_type odd = arr[std::slice(1, size / 2, 2)];

	fft(even);
	fft(odd);

    for (std::size_t i = 0; i < size / 2; ++i)
    {
        auto t = std::polar(1.0, -2 * PI * i / size) * odd[i];
        arr[i] = even[i] + t;
        arr[i + size / 2] = even[i] - t;
    }
}

void pad_to_po2(Array_type& arr)
{
	const auto size = arr.size();
	if (size <= 2)
	{
		return;
	}
	std::size_t i = 2;
	while (i < size)
	{
		i <<= 1;
	}
	if (i != size)
	{
		std::vector<std::complex<double>> v(std::begin(arr), std::end(arr));
		v.resize(i, std::complex<double>(0));
		Array_type new_arr(v.data(), v.size());
		arr.swap(new_arr);
	}
}
