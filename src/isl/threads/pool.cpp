#include <isl/thread/pool.hpp>

namespace isl::thread
{
    Pool::Pool(std::size_t size)
    {
        threads.reserve(size);

        for (std::size_t i = 0; i != size; ++i) {
            threads.emplace_back(std::mem_fn(&Pool::worker), this, &runFlag);
        }
    }

    Pool::~Pool()
    {
        runFlag = false;

        for (auto &thread : threads) {
            thread.join();
        }
    }

    auto Pool::pickJob() -> Job *
    {
        return static_cast<Job *>(tasksWithoutDependencies.pop());
    }

    auto Pool::runJob(Job *job) -> void
    {
        if (job == nullptr || job->handle == nullptr) {
            return;
        }

        while (job->isRunning.test_and_set(std::memory_order_acquire)) {}

        job->isInQueue.clear(std::memory_order_relaxed);

        if (!job->handle.done()) {
            job->handle.resume();
        }

        job->isRunning.clear(std::memory_order_release);

        if (job->handle.done()) {
            job->isCompleted.test_and_set(std::memory_order_relaxed);
            onTaskDone(job);
            return;
        }

        if (job->referencesCount.load(std::memory_order_relaxed) == 0) {
            addJobToTheQueueUnique(job);
        }
    }

    auto Pool::addJobToTheQueueUnique(isl::Job *job) -> void
    {
        if (!job->isInQueue.test_and_set()) {
            tasksWithoutDependencies.push(job);
        }
    }

    auto Pool::onTaskDone(Job *job) -> void
    {
        if (job->parent != nullptr) {
            decreaseParentsReferencesCount(job->parent);
        }
    }

    auto Pool::decreaseParentsReferencesCount(Job *parent_job) -> void
    {
        auto references = parent_job->referencesCount.fetch_sub(1, std::memory_order_release);

        if (references == 1) {
            addJobToTheQueueUnique(parent_job);
        }
    }

    auto Pool::worker(const std::atomic<bool> *run_flag) -> void
    {
        using namespace std::chrono_literals;
        auto had_job_recently = false;

        while (run_flag->load(std::memory_order_relaxed) || had_job_recently) {
            auto lock = std::unique_lock{newTasksMutex};

            had_job_recently = hasNewTasks.wait_for(lock, 10ms, [this] {
                return !tasksWithoutDependencies.wasEmpty();
            });

            lock.unlock();
            runJob(pickJob());
        }
    }

    auto Pool::await(Job *job) -> void
    {
        while (true) {
            if (!job->isCompleted.test(std::memory_order_acquire)) {
                runJob(pickJob());
                continue;
            }

            break;
        }
    }

    auto Pool::submit(Job *job) -> void
    {
        tasksWithoutDependencies.push(job);
        hasNewTasks.notify_one();
    }

    auto Pool::stop() -> void
    {
        runFlag = false;
    }
}// namespace isl::thread
