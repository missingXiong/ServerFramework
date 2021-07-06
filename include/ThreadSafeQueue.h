#pragma once

#include <iostream>
#include <thread>
#include <queue>
#include <memory>
#include <mutex>
#include <condition_variable>

#include "noncopyable.h"

template<typename T>
class thread_safe_queue : public noncopyable     // a thread safe queue
{
public:
	thread_safe_queue() {}

	// thread_safe_queue(const thread_safe_queue&) = delete;
	// thread_safe_queue& operator = (const thread_safe_queue&) = delete;

	void push(T new_value);

	bool try_pop(T& value);
	std::shared_ptr<T> try_pop();  

	void wait_and_pop(T& value);
	std::shared_ptr<T> wait_and_pop();

	bool empty() const;

private:
	mutable std::mutex mut_;  
	std::queue<std::shared_ptr<T>> data_queue_;
	std::condition_variable data_cond;
};