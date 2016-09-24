#ifndef CONCURRENCY_RING_BUFFER_HPP_
#define CONCURRENCY_RING_BUFFER_HPP_

#include <array>
#include <atomic>
#include <mutex>
#include <type_traits>
#include <memory>
#include <utility>

/**
 * @brief Threadsafe ring buffer based on an underlying array
 *
 * T must have a default constructor
 * T must have a copy constructor
 * T must have a nothrow destructor
 * T must have either a nothrow copy assignment, a nothrow move assignment or a nothrow swap
 */
template <typename T, unsigned N>
class Threadsafe_ring_buffer
{
public:
    typedef T element_type;

    /**
     * Behavior is undefined if the buffer is empty
     * Exception safety: strong guarantee
     *
     * @return The front element of the buffer
     */
	T front() const;
    /**
     * Exception safety: strong guarantee
     *
     * @return A pointer to a copy of the buffer's front element or nullptr if the buffer is empty
     */
    std::unique_ptr<T> try_front() const;
    /**
     * Adds an element to the buffer's back
     *
     * Exception safety: strong guarantee
     */
    template <typename U = T, typename std::enable_if_t<std::is_nothrow_copy_assignable<U>::value>* = nullptr>
	void push(const T&);
    /**
     * Adds an element to the buffer's back
     *
     * Exception safety: strong guarantee
     */
    template <typename U = T, typename std::enable_if_t<std::is_nothrow_move_assignable<U>::value>* = nullptr>
	void push(T&&);
    /**
     * Adds an element to the buffer's back
     *
     * Exception safety: strong guarantee if swap(T, T) doesn't throw
     */
    template <typename U = T, typename std::enable_if_t<!std::is_nothrow_copy_assignable<U>::value &&
    		!std::is_nothrow_move_assignable<U>::value>* = nullptr>
    void push(T);
    /**
     * @brief Removes the front element of the buffer
     *
     * Behavior is undefined if the buffer is empty
     * Exception safety: basic guarantee
     *
     * @return The removed element of the buffer
     */
	T pop();
    /**
     * @brief Removes the front element of the buffer
     *
     * Exception safety: strong guarantee
     *
     * @return A pointer to a copy of the buffer's front element or nullptr if the buffer is empty
     */
    std::unique_ptr<T> try_pop();
	bool empty() const noexcept;
private:
	typedef typename std::array<T, N>::iterator Iterator;

	void advance() noexcept;

	std::array<T, N> buffer_;
	Iterator data_begin_ = buffer_.begin();
	Iterator data_end_ = buffer_.begin();
	std::atomic<unsigned> size_{0};
	mutable std::mutex mutex_;
};

template <typename T, unsigned N>
void Threadsafe_ring_buffer<T, N>::advance() noexcept
{
	if (data_end_ == buffer_.end())
	{
		data_end_ = buffer_.begin();
	}
	if (data_end_ == data_begin_ && size_.load(std::memory_order_relaxed) == N)
	{
		++data_begin_;
        if (data_begin_ == buffer_.end())
        {
            data_begin_ = buffer_.begin();
        }
	}
	if (size_.load(std::memory_order_relaxed) != N)
	{
		size_.fetch_add(1, std::memory_order_release);
	}
}

template <typename T, unsigned N>
template <typename U, typename std::enable_if_t<std::is_nothrow_copy_assignable<U>::value>*>
void Threadsafe_ring_buffer<T, N>::push(const T& item)
{
	std::lock_guard<std::mutex> lk(mutex_);
	advance();
	*data_end_++ = item;
}

template <typename T, unsigned N>
template <typename U, typename std::enable_if_t<std::is_nothrow_move_assignable<U>::value>*>
void Threadsafe_ring_buffer<T, N>::push(T&& item)
{
	std::lock_guard<std::mutex> lk(mutex_);
	advance();
	*data_end_++ = std::move(item);
}

template <typename T, unsigned N>
template <typename U, typename std::enable_if_t<!std::is_nothrow_copy_assignable<U>::value &&
		!std::is_nothrow_move_assignable<U>::value>*>
void Threadsafe_ring_buffer<T, N>::push(T item)
{
    std::lock_guard<std::mutex> lk(mutex_);
	advance();
    using std::swap;
    swap(*data_end_++, item);
}

template <typename T, unsigned N>
T Threadsafe_ring_buffer<T, N>::pop()
{
	std::lock_guard<std::mutex> lk(mutex_);
    auto item = *data_begin_;
	++data_begin_;
	if (data_begin_ == buffer_.end())
	{
		data_begin_ = buffer_.begin();
	}
	size_.fetch_sub(1, std::memory_order_release);
    return item;
}

template <typename T, unsigned N>
std::unique_ptr<T> Threadsafe_ring_buffer<T, N>::try_pop()
{
	std::lock_guard<std::mutex> lk(mutex_);
    if (empty())
    {
        return nullptr;
    }
    auto ptr = std::make_unique<T>(*data_begin_);
	++data_begin_;
	if (data_begin_ == buffer_.end())
	{
		data_begin_ = buffer_.begin();
	}
	size_.fetch_sub(1, std::memory_order_release);
    return ptr;
}

template <typename T, unsigned N>
T Threadsafe_ring_buffer<T, N>::front() const
{
	std::lock_guard<std::mutex> lk(mutex_);
	return *data_begin_;
}

template <typename T, unsigned N>
bool Threadsafe_ring_buffer<T, N>::empty() const noexcept
{
	return size_.load(std::memory_order_acquire) == 0;
}

template <typename T, unsigned N>
std::unique_ptr<T> Threadsafe_ring_buffer<T, N>::try_front() const
{
    std::lock_guard<std::mutex> lk(mutex_);
    if (empty())
    {
        return nullptr;
    }
    return std::make_unique<T>(*data_begin_);
}

#endif
