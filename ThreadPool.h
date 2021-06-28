#pragma once
#include <iostream>
#include <vector>
#include <memory>
#include <thread>
#include <functional>
#include "ThreadSafeQueue.h"

struct ThreadPoolTask 
{
	std::function<void(std::shared_ptr<void>)> func;
	std::shared_ptr<void> args;
};

class ThreadPool // work threadpool
{
public:
	typedef ThreadPoolTask Task;

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
