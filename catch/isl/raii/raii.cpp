#include <isl/detail/debug/debug.hpp>
import isl;

TEST_CASE("RaiiTest", "[Raii]")
{
    // NOLINTBEGIN

    DEBUG_VAR test_variable = 10;
    {
        auto raii = isl::Raii{
            [&test_variable]() {
                test_variable = 42;
            },
            [&test_variable]() {
                test_variable = 10;
            }};

        auto raii2 = std::move(raii);

        REQUIRE(test_variable == 42);
    }
    // NOLINTEND

    REQUIRE(test_variable == 10);
}
