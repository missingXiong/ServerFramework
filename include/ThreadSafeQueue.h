#pragma once

#include <iostream>
#include <thread>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "noncopyable.h"

template <typename T>
class thread_safe_queue : public noncopyable // a thread safe queue
{
public:
	thread_safe_queue() {}

	// thread_safe_queue(const thread_safe_queue&) = delete;
	// thread_safe_queue& operator = (const thread_safe_queue&) = delete;

	void push(T new_value)
	{
		std::shared_ptr<T> pData(std::make_shared<T>(std::move(new_value)));
		{
			std::lock_guard<std::mutex> lk(mut_);
			data_queue_.push(pData);
		}
		data_cond.notify_one();
	}

	bool try_pop(T &value)
	{
		std::unique_lock<std::mutex> lk(mut_);
		if (data_queue_.empty())
			return false;
		value = std::move(*data_queue_.front());
		;
		data_queue_.pop();
		return true;
	}
	
	std::shared_ptr<T> try_pop()
	{
		std::unique_lock<std::mutex> lk(mut_);
		if (data_queue_.empty())
			return std::shared_ptr<T>();
		std::shared_ptr<T> res = data_queue_.front();
		data_queue_.pop();
		return res;
	}

	void wait_and_pop(T &value)
	{
		std::unique_lock<std::mutex> lk(mut_);
		data_cond.wait(lk, [&]()
					   { return !data_queue_.empty(); });
		value = std::move(*data_queue_.front());
		data_queue_.pop();
	}

	std::shared_ptr<T> wait_and_pop()
	{
		std::unique_lock<std::mutex> lk(mut_);
		data_cond.wait(lk, [&]()
					   { return !data_queue_.empty(); });
		std::shared_ptr<T> res = data_queue_.front();
		data_queue_.pop();
		return res;
	}

	bool empty() const
	{
		std::lock_guard<std::mutex> lk(mut_);
		return data_queue_.empty();
	}

private:
	mutable std::mutex mut_;
	std::queue<std::shared_ptr<T>> data_queue_;
	std::condition_variable data_cond;
};