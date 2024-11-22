#include <isl/detail/debug/debug.hpp>
#include <isl/thread/lockfree/stack.hpp>
#include <latch>
#include <mutex>
#include <thread>

// NOLINTBEGIN

template<typename T>
struct ValueNode : public isl::thread::lock_free::StackNode
{
    T value;
};

static auto createVector(std::size_t from, std::size_t to) -> std::vector<ValueNode<std::size_t>>
{
    auto result = std::vector<ValueNode<std::size_t>>{};
    result.reserve(to - from);

    for (auto i = from; i != to; ++i) {
        result.emplace_back(ValueNode<std::size_t>{.value = i});
    }

    return result;
}

static auto waitThreads(std::vector<std::thread> &threads) -> void
{
    for (auto &thread : threads) {
        thread.join();
    }
}

TEST_CASE("LockFreeStack", "[LockFree]")
{
    using namespace std::chrono_literals;

    static constexpr std::size_t task_multiplier = 5000;

    auto queue = isl::thread::lock_free::Stack{};
    auto pushers_threads = std::vector<std::thread>{};
    auto poppers_threads = std::vector<std::thread>{};
    auto popped_value = std::vector<std::size_t>{};
    auto popped_values_lock = std::mutex{};
    auto threads_count = std::max<std::size_t>(1, std::thread::hardware_concurrency() / 2);
    auto latch = std::latch(static_cast<std::ptrdiff_t>(threads_count * 2));
    auto pushers_finished = std::atomic<std::size_t>{0};

    auto created_data = std::vector<std::vector<ValueNode<std::size_t>>>{};
    created_data.reserve(threads_count);

    for (std::size_t i = 0; i != threads_count; ++i) {
        pushers_threads.emplace_back(
            [&queue, i, &created_data, &latch, &pushers_finished]() mutable {
                auto &nodes = created_data.emplace_back(
                    createVector(i * task_multiplier, (i + 1) * task_multiplier));

                latch.arrive_and_wait();

                for (auto &node : nodes) {
                    queue.push(&node);
                }

                pushers_finished.fetch_add(1, std::memory_order_release);
            });


        poppers_threads.emplace_back([&queue, &popped_value, &popped_values_lock, &latch,
                                      &pushers_finished, threads_count] {
            auto locally_popped = std::vector<std::size_t>{};
            latch.arrive_and_wait();

            while (true) {
                if (queue.contained() == 0 &&
                    pushers_finished.load(std::memory_order_acquire) == threads_count) {
                    break;
                }

                auto *node = queue.pop();

                if (node == nullptr) {
                    continue;
                }

                locally_popped.push_back(static_cast<ValueNode<std::size_t> *>(node)->value);
            }

            auto lock = std::scoped_lock{popped_values_lock};
            popped_value.insert(popped_value.end(), locally_popped.begin(), locally_popped.end());
        });
    }

    waitThreads(poppers_threads);
    waitThreads(pushers_threads);

    REQUIRE(popped_value.size() == threads_count * task_multiplier);

    std::ranges::sort(popped_value);

    for (std::size_t i = 0; i != threads_count * task_multiplier; ++i) {
        REQUIRE(popped_value.at(i) == i);
    }
}

// NOLINTEND
