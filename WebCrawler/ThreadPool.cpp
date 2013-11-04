//
//  ThreadPool.cpp
//  WebCrawler
//
//  Created by dongshihao on 13-11-4.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#include "ThreadPool.h"

void semaphore::Wait()
{
    std::unique_lock<std::mutex> lock (m_mutex) ;
    
    if ( --m_count < 0)
    { // count is not enough ?
        m_condition.wait(lock, [&]()->bool{ return m_wakeups > 0;}); // suspend and wait ...
        
        --m_wakeups;  // ok, me wakeup !
    }
}

void semaphore::Signal()
{
    std::lock_guard<std::mutex> lock (m_mutex) ;
    
    if( ++m_count <= 0)
    { // have some thread suspended ?
        ++m_wakeups;
        
        m_condition.notify_one(); // notify one !
    }
}


/////////////////////

void threadpool::Start(size_t thread_mum)
{
    assert(m_thread.empty());
    
    m_running.store(true);
    
    for(size_t i = 0 ; i < thread_mum ; ++i)
    {
        m_thread.push_back( std::thread( std::bind(&threadpool::runInThread , this) ) );
    }
}

void threadpool::AddTask(TaskType &&task)
{
    if (m_thread.empty())
    {
        task();
    }
    else
    {
        std::lock_guard<std::mutex> lock( m_mutex );
        
        m_task.push( std::move(task) );
        
        m_cond.notify_all();
    }
}

void threadpool::Stop()
{
    m_running.store(false);
    m_cond.notify_all();
    
    for(auto& i : m_thread)
        i.join();
}

void threadpool::runInThread()
{
    try {
        while (m_running) {
            
            TaskType task( getTask() );
            
            /// Run Task
            if( task )
            {
                task();
            }
        }
    } catch (const std::exception& e) {
        std::lock_guard<std::mutex> lock ( m_mutex_print );
        std::cout << "Thread Pool Exception : " << e.what() << std::endl;
    }
}

threadpool::TaskType threadpool::getTask()
{
    { /// Wait For WakeUp
        std::unique_lock<std::mutex> u_lock( m_mutex_wait );
        m_cond.wait(u_lock , [&]()->bool { return !(m_running && m_task.empty()) ; });
    }
    
    std::lock_guard<std::mutex> lock( m_mutex );
    
    TaskType task;
    if ( !m_task.empty() )
    {
        task = m_task.front();
        m_task.pop();
    }
    return task;
}
