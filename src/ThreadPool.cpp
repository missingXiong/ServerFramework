#include "../include/ThreadPool.h"

template<typename T>
ThreadPool<T>::ThreadPool(int threadnum /*= std::thread::hardware_concurrency()*/)
	: started_(false), threadNum_(threadnum)
{

}

template<typename T>
ThreadPool<T>::~ThreadPool()
{
	std::cout << "Clear up the thread pool" << std::endl;

	stop();
	for (int i = 0; i < threadNum_; i++)
	{
		if (threadList_[i].joinable())
			threadList_[i].join();
	}
}

template<typename T>
void ThreadPool<T>::start()
{
	if (threadNum_ > 0)
	{
		started_ = true;
		for (int i = 0; i < threadNum_; i++)
		{
			threadList_.emplace_back(std::thread(&ThreadPool::work, this));
		}
	}
}

template<typename T>
void ThreadPool<T>::stop()
{
	started_ = false;
}

template<typename T>
void ThreadPool<T>::addTask(T task)
{
	taskqueue_.push(task);
}

template<typename T>
void ThreadPool<T>::work()
{
	while (started_)
	{
		T task;
		if (taskqueue_.try_pop(task))
			task();
		else
			std::this_thread::yield();
	}
}

