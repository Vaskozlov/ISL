#ifndef ISL_PROJECT_QUEUE_HPP
#define ISL_PROJECT_QUEUE_HPP

#include <atomic>
#include <isl/isl.hpp>
#include <mutex>

namespace isl::thread::lock_free
{
    struct StackNode
    {
        StackNode *next;
    };

    class Stack
    {
    private:
        std::atomic<StackNode *> top;
        std::atomic<std::size_t> currentSize;

    public:
        auto wasEmpty() const noexcept -> bool
        {
            return top.load(std::memory_order_relaxed) == nullptr;
        }

        auto contained() const noexcept -> std::size_t
        {
            return currentSize.load(std::memory_order_relaxed);
        }

        auto push(StackNode *node) noexcept -> void
        {
            StackNode *old_head;

            do {
                old_head = top.load(std::memory_order_relaxed);
                node->next = old_head;
            } while (!top.compare_exchange_weak(
                old_head, node, std::memory_order_release, std::memory_order_relaxed));

            currentSize.fetch_add(1);
        }

        auto pop() noexcept -> StackNode *
        {
            StackNode *old_head = nullptr;

            do {
                old_head = top.load(std::memory_order_relaxed);
            } while (old_head != nullptr && !top.compare_exchange_weak(
                                                old_head,
                                                old_head->next,
                                                std::memory_order_release,
                                                std::memory_order_relaxed));

            if (old_head != nullptr) {
                currentSize.fetch_sub(1);
            }

            return old_head;
        }
    };
}// namespace isl::thread::lock_free

#endif /* ISL_PROJECT_QUEUE_HPP */
