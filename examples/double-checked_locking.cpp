#include <iostream>
#include <thread>			
#include <condition_variable>
#include <atomic>
#include <mutex>

std::atomic<bool> go_flag{false};
std::atomic<bool> done{false};
std::mutex cv_mutex;
std::mutex check_mutex;
std::condition_variable cond;

unsigned i = 0;

void do_stuff()
{
	++i;
}
			
void worker()
{
	// The following is done only to let the threads start working at the same time
	std::unique_lock<decltype(cv_mutex)> lk(cv_mutex);
	cond.wait(lk, [](){ return go_flag.load(); });
	
	if (!done.load(std::memory_order_acquire)) // first check is done without locking, with an atomic read
											   // that synchronized with the atomic write
	{
		std::lock_guard<std::mutex> lk(check_mutex); // one or more threads can reach this point, the lock is necessary 
													 // to make the second check and the following operations atomic
		if (!done.load(std::memory_order_relaxed)) // as we are under the lock's protection, this read can't happen concurrently 
												   // with the following write - we can relax the memory ordering constraints
		{
			do_stuff();
			done.store(std::memory_order_release); // happens-before the first read
		}
	}
}
			

int main()
{
	std::cout << "Value of i: " << i << std::endl;
	std::cout << "Starting threads.." << std::endl;
	
	std::thread t1(worker);
	std::thread t2(worker);
	std::thread t3(worker);
	std::thread t4(worker);
	
	go_flag = true;
	cond.notify_all();
	
	t1.join();
	t2.join();
	t3.join();
	t4.join();
	
	std::cout << "Threads finished" << std::endl;
	std::cout << "Value of i: " << i << std::endl;
}			
	
			
			
			
			
