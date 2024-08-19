#include <chrono>
#include <isl/detail/debug/debug.hpp>
#include <isl/thread/lockfree/spsc_pipe.hpp>
#include <latch>
#include <thread>

using Pipe = isl::thread::lock_free::SpscPipe<std::size_t, 4096>;

static auto pipeProducer(Pipe &pipe, std::size_t count, std::latch &latch) -> void
{
    latch.arrive_and_wait();
    auto begin = std::chrono::high_resolution_clock::now();

    for (std::size_t j = 0; j != 100; ++j) {
        for (std::size_t i = 0; i != count; ++i) {
            auto value_to_push = i;
            while (!pipe.tryPush(value_to_push)) {}
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    fmt::println(
        "Produced in {}ms",
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
}

static auto pipeConsumer(Pipe &pipe, std::size_t count, std::latch &latch) -> void
{
    latch.arrive_and_wait();
    auto begin = std::chrono::high_resolution_clock::now();

    for (std::size_t j = 0; j != 100; ++j) {
        for (std::size_t i = 0; i != count; ++i) {
            std::size_t expected = i;
            std::size_t value = 0;

            while (!pipe.tryPop(value)) {
                std::this_thread::yield();
            }

            REQUIRE(value == expected);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();
    fmt::println(
        "Consumed in {}ms",
        std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count());
}

TEST_CASE("SpscPipe", "[LockFree]")
{
    auto pipe = Pipe{};
    auto latch = std::latch(2);
    auto producer = std::thread{pipeProducer, std::ref(pipe), 4096 * 4, std::ref(latch)};
    auto consumer = std::thread{pipeConsumer, std::ref(pipe), 4096 * 4, std::ref(latch)};

    producer.join();
    consumer.join();
}
