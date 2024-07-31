#include <isl/detail/debug/debug.hpp>
#include <isl/thread/pool.hpp>

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
