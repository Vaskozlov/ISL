#include <isl/detail/debug/debug.hpp>

import isl;

TEST_CASE("JoinInts", "[Join]")
{
    DEBUG_VAR vector_of_int = std::vector<int>{10, 20, 30};// NOLINT

    REQUIRE(
        isl::join(
            vector_of_int,
            [](auto &&elem) {
                return std::to_string(elem);
            },
            ", ") == "10, 20, 30");

    REQUIRE(isl::join(vector_of_int, ", ") == "10, 20, 30");
}
