#include "ThreadPool.h"

ThreadPool::ThreadPool(int threadnum /*= std::thread::hardware_concurrency()*/)
	: started_(false), threadNum_(threadnum)
{

}

ThreadPool::~ThreadPool()
{
	std::cout << "Clear up the thread pool" << std::endl;

	stop();
	for (int i = 0; i < threadNum_; i++)
	{
		if (threadList_[i].joinable())
			threadList_[i].join();
	}
}

void ThreadPool::start()
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

void ThreadPool::stop()
{
	started_ = false;
}

void ThreadPool::addTask(Task task)
{
	taskqueue_.push(task);
}

void ThreadPool::work()
{
	while (started_)
	{
		Task task;
		if (taskqueue_.try_pop(task))
			(task.func)(task.args);
		else
			std::this_thread::yield();
	}
}

