#ifndef CONCURRENCY_THREAD_JOINER_HPP_
#define CONCURRENCY_THREAD_JOINER_HPP_

#include <vector>
#include <thread>

class Thread_joiner
{
public:
	explicit Thread_joiner(std::vector<std::thread>& threads) : threads_(threads) {}
	~Thread_joiner()
	{
		for (auto& thread : threads_)
			if (thread.joinable())
				thread.join();
	}
	Thread_joiner(const Thread_joiner&) = delete;
	Thread_joiner(Thread_joiner&&) noexcept = delete;
	Thread_joiner& operator=(const Thread_joiner&) = delete;
	Thread_joiner& operator=(Thread_joiner&&) noexcept = delete;
private:
	std::vector<std::thread>& threads_;
};

#endif
