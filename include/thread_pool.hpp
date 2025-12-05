#if !defined(THREAD_POOL_HPP)
#define THREAD_POOL_HPP

#include <thread>
#include <functional>
#include <vector>
#include <queue>
#include <condition_variable>
#include <mutex>
#include <future>

class ThreadPool
{
public:
    ThreadPool(size_t threads);
    ~ThreadPool();

    template <class F, class... Args>
    auto enqueue(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>;

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop = false;
};

#endif // THREAD_POOL_HPP
