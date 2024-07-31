#include <iostream>
#include <isl/coroutine/task.hpp>
#include <isl/detail/debug/debug.hpp>

auto simpleTask() -> isl::Task<int>
{
    co_return 0;
}

auto taskAwaitingOtherTask() -> isl::Task<int>
{
    auto task = simpleTask();
    co_return co_await task;
}

TEST_CASE("SimpleTask", "[Task]")
{
    auto task = simpleTask();
    REQUIRE_THROWS(task.get());

    task.await();

    REQUIRE(task.done());
    REQUIRE(task.get() == 0);
}

TEST_CASE("TaskAwaitingTask", "[Task]")
{
    auto task = taskAwaitingOtherTask();
    REQUIRE_THROWS(task.get());

    while (!task.done()) {
        task.await();
    }

    REQUIRE(task.done());
    REQUIRE(task.get() == 0);
}
