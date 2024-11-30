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

        auto push(StackNode *node) noexcept -> void
        {
            StackNode *old_head = nullptr;

            do {
                node->next = old_head;
            } while (!top.compare_exchange_weak(
                old_head, node, std::memory_order_release, std::memory_order_relaxed));

            currentSize.fetch_add(1, std::memory_order_release);
        }

        auto pop() noexcept -> StackNode *
        {
            StackNode *old_head = top.load(std::memory_order_relaxed);

            while (
                old_head != nullptr &&
                !top.compare_exchange_weak(
                    old_head, old_head->next, std::memory_order_release, std::memory_order_relaxed)) {}

            if (old_head != nullptr) {
                currentSize.fetch_sub(1, std::memory_order_release);
            }

            return old_head;
        }
    };
}// namespace isl::thread::lock_free

#endif /* ISL_PROJECT_LOCK_FREE_STACK_HPP */
