#ifndef ISL_PROJECT_LOCK_FREE_STACK_HPP
#define ISL_PROJECT_LOCK_FREE_STACK_HPP

#include <atomic>
#include <isl/isl.hpp>
#include <mutex>

namespace isl::thread::lock_free
{
    class Stack;

    struct StackNode
    {
    private:
        friend Stack;
        StackNode *next;
    };

    class Stack
    {
    private:
        std::atomic<StackNode *> top;
        std::atomic<std::size_t> currentSize;

    public:
        [[nodiscard]] auto wasEmpty() const noexcept -> bool
        {
            return top.load(std::memory_order_acquire) == nullptr;
        }

        [[nodiscard]] auto contained() const noexcept -> std::size_t
        {
            return currentSize.load(std::memory_order_acquire);
        }

        [[nodiscard]] auto wasOnTop() noexcept -> StackNode *
        {
            return top.load(std::memory_order_acquire);
        }

        [[nodiscard]] auto wasOnTop() const noexcept -> const StackNode *
        {
            return top.load(std::memory_order_acquire);
        }

        auto push(StackNode *node) noexcept -> void;

        auto pop() noexcept -> StackNode *;
    };
}// namespace isl::thread::lock_free

#endif /* ISL_PROJECT_LOCK_FREE_STACK_HPP */
