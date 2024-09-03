#ifndef ISL_PROJECT_SPIN_LOCK_HPP
#define ISL_PROJECT_SPIN_LOCK_HPP

#include <atomic>

namespace isl::thread
{
    class SpinLock
    {
    private:
        std::atomic_flag flag{false};

    public:
        auto lock() -> void
        {
            while (flag.test_and_set(std::memory_order_acquire)) {}
        }

        [[nodiscard]] auto tryLock() -> bool
        {
            return !flag.test_and_set(std::memory_order_acquire);
        }

        auto unlock() -> void
        {
            flag.clear(std::memory_order_release);
        }
    };
}// namespace isl::thread

#endif /* ISL_PROJECT_SPIN_LOCK_HPP */
