#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <functional>
#include "ThreadSafeQueue.h"

template<typename T>
class ThreadPool // work threadpool
{
public:
	ThreadPool(int threadnum = std::thread::hardware_concurrency());
	~ThreadPool();

	void start();
	void stop();
	void addTask(T task);
	
private:
	bool started_;
	int threadNum_;
	thread_safe_queue<T> taskqueue_;
	std::vector<std::thread> threadList_;

	void work();
};
