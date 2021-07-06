#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <functional>
#include "ThreadSafeQueue.h"


class ThreadPool // work threadpool
{
public:
	typedef std::function<void()> Task;

	ThreadPool(int threadnum = std::thread::hardware_concurrency());
	~ThreadPool();

	void start();
	void stop();
	void addTask(Task task);
	
private:
	bool started_;
	int threadNum_;
	thread_safe_queue<Task> taskqueue_;
	std::vector<std::thread> threadList_;

	void work();
};
