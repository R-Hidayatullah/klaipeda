#include "thread_pool.hpp"

ThreadPool::ThreadPool(size_t threads)
{
    for (size_t i = 0; i < threads; ++i)
    {
        workers.emplace_back([this]()
                             {
            for (;;) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->queue_mutex);
                    this->condition.wait(lock, [this]{ return this->stop || !this->tasks.empty(); });
                    if (this->stop && this->tasks.empty()) return;
                    task = std::move(this->tasks.front());
                    this->tasks.pop();
                }
                task();
            } });
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();
    for (auto &w : workers)
        if (w.joinable())
            w.join();
}

template <class F, class... Args>
auto ThreadPool::enqueue(F &&f, Args &&...args)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task_ptr = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task_ptr->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        tasks.emplace([task_ptr]()
                      { (*task_ptr)(); });
    }
    condition.notify_one();
    return res;
}

// Explicit instantiation of the template for common signatures is not necessary here
// but the file must compile; include the template in header as inline usually.
// To keep it simple, define enqueue implementation in header in real projects.
