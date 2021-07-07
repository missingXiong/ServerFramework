#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <functional>
#include "ThreadSafeQueue.h"

template <typename T>
class ThreadPool // work threadpool
{
public:
	ThreadPool(int threadnum = std::thread::hardware_concurrency())
		: started_(false), threadNum_(threadnum)
	{
	}

	~ThreadPool()
	{
		std::cout << "Clear up the thread pool" << std::endl;

		stop();
		for (int i = 0; i < threadNum_; i++)
		{
			if (threadList_[i].joinable())
				threadList_[i].join();
		}
	}

	void start()
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
	
	void stop()
	{
		started_ = false;
	}

	void addTask(T task)
	{
		taskqueue_.push(task);
	}

private:
	bool started_;
	int threadNum_;
	thread_safe_queue<T> taskqueue_;
	std::vector<std::thread> threadList_;

	void work()
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
};
