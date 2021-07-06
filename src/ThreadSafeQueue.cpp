#include "../include/ThreadSafeQueue.h"

template<typename T>
void thread_safe_queue<T>::push(T new_value)
{
	std::shared_ptr<T> pData(std::make_shared<T>(std::move(new_value)));
	{
		std::lock_guard<std::mutex> lk(mut_);
		data_queue_.push(pData);
	}
	data_cond.notify_one();
}

template<typename T>
bool thread_safe_queue<T>::try_pop(T& value)
{
	std::unique_lock<std::mutex> lk(mut_);
	if (data_queue_.empty())
		return false;
	value = std::move(*data_queue_.front());;
	data_queue_.pop();
	return true;
}

template<typename T>
std::shared_ptr<T> thread_safe_queue<T>::try_pop()   // return nullptr if no value to retrieve
{
	std::unique_lock<std::mutex> lk(mut_);
	if (data_queue_.empty())
		return std::shared_ptr<T>();
	std::shared_ptr<T> res = data_queue_.front();
	data_queue_.pop();
	return res;
}

template<typename T>
void thread_safe_queue<T>::wait_and_pop(T& value)
{
	std::unique_lock<std::mutex> lk(mut_);
	data_cond.wait(lk, [&]() { return !data_queue_.empty(); });
	value = std::move(*data_queue_.front());
	data_queue_.pop();
}

template<typename T>
std::shared_ptr<T> thread_safe_queue<T>::wait_and_pop()
{
	std::unique_lock<std::mutex> lk(mut_);
	data_cond.wait(lk, [&]() { return !data_queue_.empty(); });
	std::shared_ptr<T> res = data_queue_.front();
	data_queue_.pop();
	return res;
}

template<typename T>
bool thread_safe_queue<T>::empty() const
{
	std::lock_guard<std::mutex> lk(mut_);
	return data_queue_.empty();
}