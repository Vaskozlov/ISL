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

    auto Pool::pickJob() -> JobIterator
    {
        const auto lock = std::scoped_lock{tasksMutex};

        if (readyTasks.empty()) {
            return readyTasks.end();
        }

        for (auto job_it = readyTasks.begin(), it_end = readyTasks.end(); job_it != it_end;
             ++job_it) {
            if (!job_it->isRunning.test_and_set(std::memory_order_relaxed)) {
                return job_it;
            }
        }

        return readyTasks.end();
    }

    auto Pool::doWork() -> void
    {
        auto job = pickJob();

        if (job == readyTasks.end()) {
            return;
        }

        auto task_handle = job->handle;

        if (!task_handle.done()) {
            task_handle.resume();
        }

        if (task_handle.done() && !job->isCompleted.test(std::memory_order_relaxed)) {
            onTaskDone(job);
            return;
        }

        job->isRunning.clear(std::memory_order_relaxed);
    }

    auto Pool::onTaskDone(JobIterator job) -> void
    {
        auto parent_job = job->parent;
        const auto lock = std::scoped_lock{tasksMutex};

        if (parent_job != tasks.end()) {
            decreaseParentsReferencesCount(parent_job);
        }

        handleToJob.erase(job->taskId);
        [[maybe_unused]] const auto *_ = readyTasks.release(job).release();

        job->isRunning.clear(std::memory_order_relaxed);
        job->isCompleted.test_and_set(std::memory_order_relaxed);
    }

    auto Pool::decreaseParentsReferencesCount(JobIterator parent_job) -> void
    {
        auto parent_references =
            parent_job->referencesCount.fetch_sub(1, std::memory_order_relaxed);

        if (parent_references == 1) {
            readyTasks.insertFront(tasks.release(parent_job));
        }
    }

    auto Pool::worker(const std::atomic<bool> *run_flag) -> void
    {
        using namespace std::chrono_literals;

        while (run_flag->load(std::memory_order_relaxed)) {
            doWork();
        }
    }

    auto Pool::removeFromPool(JobIterator job) -> void
    {
        while (true) {
            if (!job->isCompleted.test(std::memory_order_relaxed)) {
                doWork();
                continue;
            }

            std::destroy_at(job.getNodePtr());
            break;
        }
    }

    auto Pool::submit(coro::coroutine_handle<> task_handle, Id task_id, Id parent_id) -> JobIterator
    {
        const auto tasks_lock = std::scoped_lock{tasksMutex};

        auto parent_job = getParentJob(parent_id);
        increaseParentsReferencesCount(parent_job);

        auto job_it = readyTasks.emplaceFront(task_handle, parent_job, task_id);
        handleToJob.emplace(task_id, job_it);

        return job_it;
    }

    auto Pool::getParentJob(Id parent_id) -> JobIterator
    {
        auto parent_job_map_it = handleToJob.find(parent_id);

        if (parent_job_map_it == handleToJob.end()) {
            return tasks.end();
        }

        return parent_job_map_it->second;
    }

    auto Pool::increaseParentsReferencesCount(JobIterator parent) -> void
    {
        if (parent != tasks.end()) {
            auto parent_references =
                parent->referencesCount.fetch_add(1, std::memory_order_relaxed);

            if (parent_references == 0) {
                tasks.insertFront(readyTasks.release(parent));
            }
        }
    }

    auto Pool::stop() -> void
    {
        runFlag = false;
    }
}// namespace isl::thread
