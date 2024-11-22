#include "isl/thread/lockfree/stack.hpp"

namespace isl::thread::lock_free
{
    auto Stack::push(StackNode *node) noexcept -> void
    {
        StackNode *old_head = nullptr;

        do {
            node->next = old_head;
        } while (!top.compare_exchange_weak(
            old_head, node, std::memory_order_release, std::memory_order_relaxed));

        currentSize.fetch_add(1, std::memory_order_release);
    }

    auto Stack::pop() noexcept -> StackNode *
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
}// namespace isl::thread::lock_free
