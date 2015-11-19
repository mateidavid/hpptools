/// @author    Matei David, Ontario Institute for Cancer Research
/// @version   1.0
/// @date      2015
/// @copyright MIT Public License
///
/// A C++11 thread pool.
///
/// Inspired by:
///   http://stackoverflow.com/a/29742586/717706

#ifndef __TPOOL_HPP
#define __TPOOL_HPP

#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <functional>
#include <chrono>

#include "logger.hpp"

namespace tpool
{

class tpool
{
public:
    // create threads
    tpool(unsigned num_threads = 0) { resize(num_threads); }

    // wait for work to be done, then destroy threads
    ~tpool() { clear(); }

    void clear()
    {
        LOG("tpool", info) << "clearing thread pool" << std::endl;
        {
            // Unblock any threads and tell them to stop
            std::unique_lock< std::mutex > l(_queue_mtx);
            _shutdown = true;
            _work_cv.notify_all();
        }
        // Wait for all threads to stop
        for (auto& thread : _threads)
        {
            thread.join();
        }
        _threads.clear();
    }

    // change the number of threads
    void resize(unsigned num_threads)
    {
        if (num_threads == size())
        {
            return;
        }
        if (size() > 0)
        {
            clear();
        }
        if (num_threads == 0)
        {
            return;
        }
        LOG("tpool", info) << "creating pool of " << num_threads << " threads" << std::endl;
        // Create the specified number of threads
        _n_idle = 0;
        _shutdown = false;
        _threads.reserve(num_threads);
        for (unsigned i = 0; i < num_threads; ++i)
        {
            _threads.emplace_back(std::bind(&tpool::worker, this, i));
        }
    }

    // number of threads
    unsigned size() const { return _threads.size(); }

    // add a job to process
    // the single argument passed will be the thread id
    void add_job(std::function< void(unsigned) > job)
    {
        if (size() > 0)
        {
            // Place a job on the queue and unblock a thread
            std::unique_lock< std::mutex > l(_queue_mtx);
            _jobs.emplace(std::move(job));
            _work_cv.notify_one();
        }
        else
        {
            // no threads in pool; run job in main thread, with argument 0
            job(0);
        }
    }

    // wait until the jobs are done
    void wait_jobs()
    {
        if (size() > 0)
        {
            std::unique_lock< std::mutex > l(_queue_mtx);
            LOG("tpool", debug) << "start waiting for jobs" << std::endl;
            while (not (_jobs.empty() and _n_idle == _threads.size()))
            {
                _wait_cv.wait(l);
            }
            LOG("tpool", debug) << "end waiting for jobs" << std::endl;
        }
    }

private:
    void worker(unsigned i)
    {
        std::function< void(unsigned) > job;
        LOG("tpool", debug) << "thread " << i << ": start" << std::endl;
        while (true)
        {
            {
                LOG("tpool", debug) << "thread " << i << ": waiting for job" << std::endl;
                std::unique_lock< std::mutex > l(_queue_mtx);
                ++_n_idle;
                while (not _shutdown and _jobs.empty())
                {
                    _wait_cv.notify_all();
                    _work_cv.wait(l);
                }
                if (_jobs.empty())
                {
                    // No jobs to do and we are shutting down
                    LOG("tpool", debug) << "thread " << i << ": end" << std::endl;
                    return;
                }
                job = std::move(_jobs.front());
                _jobs.pop();
                --_n_idle;
                LOG("tpool", debug) << "thread " << i << ": starting job" << std::endl;
            }
            job(i);
        }
    }

    // data
    std::queue< std::function< void(unsigned) > > _jobs;
    std::vector< std::thread > _threads;
    std::mutex _queue_mtx;
    std::condition_variable _work_cv;
    std::condition_variable _wait_cv;
    unsigned _n_idle;
    bool _shutdown;
}; // class tpool

} // namespace tpool

#endif

#ifdef SAMPLE_TPOOL
/*

g++ -std=c++11 -pthread -D SAMPLE_TPOOL -x c++ tpool.hpp -o test-tpool

*/

using namespace std;
using namespace std::placeholders;

void zzz(unsigned tid, unsigned i)
{
    // A silly job for demonstration purposes
    LOG(info) << "tid=" << tid << " i=" << i << " sleep=" << i % 3 << endl;
    this_thread::sleep_for(chrono::seconds(i % 3));
}

int main()
{
    logger::Logger::set_default_level(logger::debug);
    // Create two threads
    tpool::tpool p(2);
    for (unsigned r = 0; r < 2; ++r)
    {
        LOG(info) << "start round " << r << endl;
        // Assign them 4 jobs
        for (unsigned i = 0; i < 10; i += 2)
        {
            p.add_job(bind(zzz, _1, i));
            p.add_job([&,i] (unsigned tid) { zzz(tid, i + 1); });
        }
        p.wait_jobs();
        LOG(info) << "end iteration " << r << endl;
    }
}

#endif
