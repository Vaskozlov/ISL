#include <functional>
#include <isl/thread/pool.hpp>

namespace isl::thread
{
    Pool::Pool(const std::size_t count)
    {
        startThreads(count);
    }

    Pool::~Pool()
    {
        stopAllThreads();
    }

    auto Pool::pickJob() -> Job *
    {
        return static_cast<Job *>(tasksStack.pop());
    }

    auto Pool::runJob(Job *job) -> bool
    {
        if (job == nullptr) {
            return false;
        }

        job->run();
        job->isCompleted.test_and_set(std::memory_order_release);

        return true;
    }

    auto Pool::waitForNotify() -> void
    {
        using namespace std::chrono_literals;

        auto lock = std::unique_lock{newTasksMutex};

        hasNewTasks.wait_for(lock, 10ms, [this] {
            return !tasksStack.wasEmpty();
        });

        lock.unlock();
    }

    auto Pool::worker(const std::atomic<bool> &run_flag) -> void
    {
        auto had_job_recently = false;

        while (run_flag.load(std::memory_order_relaxed) || had_job_recently) {
            waitForNotify();
            had_job_recently = runJob(pickJob());
        }
    }

    auto Pool::await(const Job *job) -> void
    {
        while (!job->isCompleted.test(std::memory_order_relaxed)) {
            runJob(pickJob());
        }
    }

    auto Pool::submit(Job *job) -> void
    {
        tasksStack.push(job);
        hasNewTasks.notify_one();
    }

    auto Pool::wereRunning() -> std::size_t
    {
        const auto lock = std::scoped_lock{threadsManipulationMutex};

        return threads.size();
    }

    auto Pool::startThreads(std::size_t count) -> void
    {
        const auto lock = std::scoped_lock{threadsManipulationMutex};

        for (std::size_t i = 0; i != count; ++i) {
            threads.emplace_back(true);

            auto &[thread, run_flag] = threads.back();
            thread = std::thread{std::mem_fn(&Pool::worker), this, std::cref(run_flag)};
        }
    }

    auto Pool::stopOneThread() -> void
    {
        const auto lock = std::scoped_lock{threadsManipulationMutex};

        if (threads.empty()) {
            return;
        }

        auto &[thread, run_flag] = threads.back();

        run_flag.store(false, std::memory_order_relaxed);
        hasNewTasks.notify_all();

        thread.join();
        threads.pop_back();
    }

    auto Pool::stopAllThreads() -> void
    {
        const auto lock = std::scoped_lock{threadsManipulationMutex};

        for (auto &[thread, run_flag] : threads) {
            run_flag.store(false, std::memory_order_relaxed);
        }

        hasNewTasks.notify_all();

        for (auto &[thread, run_flag] : threads) {
            thread.join();
        }

        threads.clear();
    }
}// namespace isl::thread
