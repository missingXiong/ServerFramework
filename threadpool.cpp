#include "threadpool.h"

template<typename T>
threadpool<T>::threadpool(int thread_num, int max_requests)
    : m_thread_num(thread_num), m_max_requests(max_requests)
{
    if (m_thread_num <= 0 || m_max_requests <= 0)
        throw std::exception();
    
    m_threads = new pthread_t[m_thread_num];
    if (!m_thread)
        throw std::exception();
    
    for (int i = 0; i < m_thread_num; i++)
    {
        if (pthread_create(m_threads + 1, NULL, worker, this) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }

        if (pthread_detach(m_threads + 1) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }
    }
}

template<typename T>
threadpool<T>::~threadpool()
{
    if (m_threads)
    {
        delete[] m_threads;
    }
    m_stop = true;
}

template<typename T>
bool threadpool<T>::append(T* request)
{
    m_queuelocker.lock();
    if (m_workqueue.size() > m_max_requests)
    {
        m_queuelocker.unlcok();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.unlock();
    m_queuestat.post();
    return true;
}

template<typename T>
void* threadpool<T>::worker(void* argv)
{
    threadpool* p = (threadpool*)argv;
    p->run();
    return p;
}

template<typename T>
void threadpool<T>::run()
{
    while (!m_stop())
    {
        m_queuestat.wait();
        m_queuelocker.lock();
        if (m_workqueue.empty())
        {
            m_queuelocker.unlock();
            continue;
        }
        T* request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.unlock();
        if (!request) 
            continue;
        
        request->process();
    }
}