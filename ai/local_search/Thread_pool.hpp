#ifndef AI_THREAD_POOL_HPP_
#define AI_THREAD_POOL_HPP_

/**
 * Original code from the book: C++ concurrency in action
 */

#include <queue>
#include <memory>
#include <thread>
#include <utility>
#include <future>
#include <vector>
#include <deque>
#include <type_traits>
#include <condition_variable>
#include "Thread_joiner.hpp"

template <typename T>
class Threadsafe_queue
{
private:
	struct Node
	{
		std::shared_ptr<T> data;
		std::unique_ptr<Node> next;
	};
public:
	Threadsafe_queue() : head_(new Node), tail_(head_.get()) {}
	Threadsafe_queue(const Threadsafe_queue&) = delete;
	Threadsafe_queue& operator=(const Threadsafe_queue&) = delete;
	bool empty();
	bool try_pop(T&);
	std::shared_ptr<T> try_pop();
	std::shared_ptr<T> wait_and_pop();
	void wait_and_pop(T&);
	void push(T);
private:
	Node* get_tail();
	std::unique_ptr<Node> pop_head();
	std::unique_lock<std::mutex> wait_for_data();
	std::unique_ptr<Node> wait_pop_head();
	std::unique_ptr<Node> wait_pop_head(T&);
	std::unique_ptr<Node> try_pop_head(T&);
	std::unique_ptr<Node> try_pop_head();
	std::unique_ptr<Node> head_;
	Node* tail_;
	std::mutex head_mutex_;
	std::mutex tail_mutex_;
	std::condition_variable data_cond;
};

template<typename T>
void Threadsafe_queue<T>::push(T value)
{
	auto new_data = std::make_shared<T>(std::move(value));
	auto node = std::make_unique<Node>();
	{
		std::lock_guard<std::mutex> tail_lock(tail_mutex_);
		tail_->data = new_data;
		Node* const new_tail = node.get();
		tail_->next = std::move(node);
		tail_ = new_tail;
	}
	data_cond.notify_one();
}

template <typename T>
typename Threadsafe_queue<T>::Node* Threadsafe_queue<T>::get_tail()
{
	std::lock_guard<std::mutex> tail_lock(tail_mutex_);
	return tail_;
}

template <typename T>
std::unique_ptr<typename Threadsafe_queue<T>::Node> Threadsafe_queue<T>::pop_head()
{
	std::unique_ptr<Node> old_head = std::move(head_);
	head_ = std::move(old_head->next);
	return old_head;
}

template <typename T>
std::unique_lock<std::mutex> Threadsafe_queue<T>::wait_for_data()
{
	std::unique_lock<std::mutex> head_lock(head_mutex_);
	data_cond.wait(head_lock, [&] { return head_.get() != get_tail(); });
	return std::move(head_lock);
}

template <typename T>
std::unique_ptr<typename Threadsafe_queue<T>::Node> Threadsafe_queue<T>::wait_pop_head()
{
	std::unique_lock<std::mutex> head_lock(wait_for_data());
	return pop_head();
}

template <typename T>
std::unique_ptr<typename Threadsafe_queue<T>::Node> Threadsafe_queue<T>::wait_pop_head(T& value)
{
	std::unique_lock<std::mutex> head_lock(wait_for_data());
	value = std::move(*head_->data);
	return pop_head();
}

template <typename T>
std::shared_ptr<T> Threadsafe_queue<T>::wait_and_pop()
{
	const std::unique_ptr<Node> old_head = wait_pop_head();
	return old_head->data;
}

template <typename T>
void Threadsafe_queue<T>::wait_and_pop(T& value)
{
	const std::unique_ptr<Node> old_head = wait_pop_head(value);
}

template <typename T>
std::unique_ptr<typename Threadsafe_queue<T>::Node> Threadsafe_queue<T>::try_pop_head()
{
	std::lock_guard<std::mutex> head_lock(head_mutex_);
	if (head_.get() == get_tail())
	{
		return std::unique_ptr<Node>();
	}
	return pop_head();
}

template <typename T>
std::unique_ptr<typename Threadsafe_queue<T>::Node> Threadsafe_queue<T>::try_pop_head(T& value)
{
	std::lock_guard<std::mutex> head_lock(head_mutex_);
	if (head_.get()==get_tail())
	{
		return std::unique_ptr<Node>();
	}
	value = std::move(*head_->data);
	return pop_head();
}

template <typename T>
bool Threadsafe_queue<T>::empty()
{
	std::lock_guard<std::mutex> head_lock(head_mutex_);
	return (head_.get() == get_tail());
}

template <typename T>
std::shared_ptr<T> Threadsafe_queue<T>::try_pop()
{
	std::unique_ptr<Node> old_head = try_pop_head();
	return old_head ? old_head->data : std::shared_ptr<T>();
}

template <typename T>
bool Threadsafe_queue<T>::try_pop(T& value)
{
	const std::unique_ptr<Node> old_head = try_pop_head(value);
	return old_head != nullptr;
}

class Function_wrapper
{
    struct Impl_base
    {
        virtual void call() = 0;
        virtual ~Impl_base() {}
    };
    template <typename F>
    struct Impl_type: Impl_base
    {
        F f;
        Impl_type(F&& f_): f(std::move(f_)) {}
        void call() { f(); }
    };
    std::unique_ptr<Impl_base> impl_;
public:
    Function_wrapper() = default;
    template <typename F>
    Function_wrapper(F&& f) : impl_(new Impl_type<F>(std::move(f))) {}
    Function_wrapper(Function_wrapper&& other) : impl_(std::move(other.impl_)) {}
    void operator()() { impl_->call(); }
    Function_wrapper& operator=(Function_wrapper&& other)
    {
        impl_ = std::move(other.impl_);
        return *this;
    }
    Function_wrapper(const Function_wrapper&) = delete;
    Function_wrapper(Function_wrapper&) = delete;
    Function_wrapper& operator=(const Function_wrapper&) = delete;
};

class Work_stealing_queue
{
	typedef Function_wrapper Data_type;
public:
	Work_stealing_queue()
	{}
	Work_stealing_queue(const Work_stealing_queue& other) = delete;
	Work_stealing_queue& operator=(const Work_stealing_queue& other) = delete;
	void push(Data_type data)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		queue_.push_front(std::move(data));
	}
	bool empty() const
	{
		std::lock_guard<std::mutex> lock(mutex_);
		return queue_.empty();
	}
	bool try_pop(Data_type& res)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (queue_.empty())
		{
			return false;
		}
		res = std::move(queue_.front());
		queue_.pop_front();
		return true;
	}
	bool try_steal(Data_type& res)
	{
		std::lock_guard<std::mutex> lock(mutex_);
		if (queue_.empty())
		{
			return false;
		}
		res = std::move(queue_.back());
		queue_.pop_back();
		return true;
	}
private:
	std::deque<Data_type> queue_;
	mutable std::mutex mutex_;
};

class Thread_pool
{
public:
	Thread_pool(unsigned num_threads = std::thread::hardware_concurrency()) : done_(false), joiner_(threads_)
    {
    	try
    	{
			for (unsigned i = 0; i < num_threads; ++i)
			{
				queues_.push_back(std::make_unique<Work_stealing_queue>());
				threads_.push_back(std::thread(&Thread_pool::worker_thread, this, i));
			}
    	}
    	catch (...)
    	{
			done_ = true;
			throw;
    	}
    }
    ~Thread_pool()
    {
        done_ = true;
    }
    template <typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type> submit(FunctionType f)
    {
        typedef typename std::result_of<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task(std::move(f));
        std::future<result_type> res(task.get_future());
        if (local_work_queue_)
        {
        	local_work_queue_->push(std::move(task));
        }
        else
        {
        	pool_work_queue_.push(std::move(task));
        }
        return res;
    }
    void run_pending_task()
    {
        Function_wrapper task;
		if (pop_task_from_local_queue(task) ||
			pop_task_from_pool_queue(task) ||
			pop_task_from_other_thread_queue(task))
		{
        	task();
        }
        else
        {
        	std::this_thread::yield();
        }
    }
private:
	void worker_thread(unsigned index)
	{
		index_ = index;
		local_work_queue_ = queues_[index].get();
		while (!done_)
		{
			run_pending_task();
		}
	}
	bool pop_task_from_local_queue(Function_wrapper& task)
	{
		return local_work_queue_ && local_work_queue_->try_pop(task);
	}
	bool pop_task_from_pool_queue(Function_wrapper& task)
	{
		return pool_work_queue_.try_pop(task);
	}
	bool pop_task_from_other_thread_queue(Function_wrapper& task)
	{
		for (unsigned i = 0; i < queues_.size(); ++i)
		{
			unsigned index = (index_ + i + 1) % queues_.size();
			if(queues_[index]->try_steal(task))
			{
			return true;
			}
		}
		return false;
	}
    std::atomic_bool done_;
    Threadsafe_queue<Function_wrapper> pool_work_queue_;
	std::vector<std::unique_ptr<Work_stealing_queue>> queues_;
    std::vector<std::thread> threads_;
    Thread_joiner joiner_;
    static thread_local Work_stealing_queue* local_work_queue_;
	static thread_local unsigned index_;
};

thread_local Work_stealing_queue* Thread_pool::local_work_queue_ = nullptr;
thread_local unsigned Thread_pool::index_ = 0;

#endif
