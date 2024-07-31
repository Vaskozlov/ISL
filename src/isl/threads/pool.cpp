#include <isl/thread/pool.hpp>

namespace isl::thread
{
    Pool::Pool(std::size_t count)
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

    auto Pool::runJob(Job *job) -> void
    {
        if (job == nullptr) {
            return;
        }

        job->run();
        onTaskDone(job);
    }

    auto Pool::onTaskDone(Job *job) -> void
    {
        if (job->parent != nullptr) {
            decreaseParentsReferencesCount(job->parent);
        }

        job->isCompleted.test_and_set(std::memory_order_relaxed);
    }

    auto Pool::decreaseParentsReferencesCount(Job *parent_job) -> void
    {
        if (parent_job->referencesCount.fetch_sub(1, std::memory_order_relaxed) == 1) {
            submit(parent_job);
        }
    }

    auto Pool::worker(ThreadInfoIterator run_flag_it) -> void
    {
        using namespace std::chrono_literals;
        auto had_job_recently = false;
        auto &[this_thread, run_flag] = *run_flag_it;

        while (run_flag.load(std::memory_order_relaxed) || had_job_recently) {
            auto lock = std::unique_lock{newTasksMutex};

            had_job_recently = hasNewTasks.wait_for(lock, 10ms, [this] {
                return !tasksStack.wasEmpty();
            });

            lock.unlock();
            runJob(pickJob());
        }
    }

    auto Pool::await(Job *job) -> void
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
            threads.emplaceFrontWithSelfIteratorAttached(true, std::mem_fn(&Pool::worker), this);
        }
    }

    auto Pool::stopOneThread() -> void
    {
        auto thread_info = isl::UniquePtr<typename PersistentStorage<ThreadInfo>::Node>{};

        {
            const auto lock = std::scoped_lock{threadsManipulationMutex};
            if (threads.empty()) {
                return;
            }

            thread_info = threads.release(threads.begin());
        }

        auto &[thread, run_flag] = thread_info->data;

        run_flag.store(false, std::memory_order_relaxed);
        hasNewTasks.notify_all();

        thread.join();
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
