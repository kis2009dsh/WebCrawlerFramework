//
//  ThreadPool.h
//  WebCrawler
//
//  Created by dongshihao on 13-11-4.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#ifndef __WebCrawler__ThreadPool__
#define __WebCrawler__ThreadPool__

#include <iostream>
#include <thread>
#include <mutex>
#include <atomic>
#include <queue>
#include <map>
#include <mutex>
#include <condition_variable>

#include <cassert>

class semaphore
{
public:
    semaphore(int value = 1): m_count{value}, m_wakeups{0} {}
    
    void Wait();
    
    void Signal() ;
    
private:
    int m_count;
    int m_wakeups;
    std::mutex m_mutex;
    std::condition_variable m_condition;
};

class threadpool
{
public:
    
    typedef std::function<void(void)> TaskType ;
    
public:
    
    threadpool() : m_running(false)
    {
        /// Nothing
    }
    
    ~threadpool()
    {
        if( m_running.load() )
            Stop() ;
    }
    
public:
    
    ///
    ///  Create Thread Pool
    ///  @param thread_num    number of thread
    ///  @brief Call runInThread
    ///
    void Start( size_t thread_mum ) ;
    
    ///
    ///  Add Task to Pool
    ///  @param task
    ///  @brief if Pool.empty() then Run it directly
    ///
    void AddTask( TaskType&& task ) ;
    
    ///
    ///  Stop All Thread in Pool
    ///  @brief Wait for join()
    ///
    void Stop() ;
    
private:
    
    void runInThread() ;
    
    TaskType getTask() ;
    
    std::atomic<bool>        m_running;
    std::mutex               m_mutex ;
    std::mutex               m_mutex_wait;
    std::mutex               m_mutex_print;
    std::condition_variable  m_cond  ;
    std::vector<std::thread> m_thread;
    std::queue<TaskType>     m_task ;
};

#endif /* defined(__WebCrawler__ThreadPool__) */
