#include <isl/detail/debug/debug.hpp>
#include <isl/thread/pool.hpp>
#include <isl/thread/scope.hpp>

TEST_CASE("PoolCreation", "[Pool]")
{
    auto pool = isl::thread::Pool{0};
    REQUIRE(pool.wereRunning() == 0);

    pool.startThreads(4);
    REQUIRE(pool.wereRunning() == 4);

    pool.stopOneThread();
    REQUIRE(pool.wereRunning() == 3);

    pool.stopAllThreads();
    REQUIRE(pool.wereRunning() == 0);
}


TEST_CASE("PoolLaunch", "[Pool]")
{
    auto pool = isl::thread::Pool{2};
    auto scope = isl::Scope(&pool);

    scope.launch([]() -> isl::Task<> {
        for (int i = 0; i != 100; ++i) {
            fmt::println("{}", i);
        }
        co_return;
    }());

}