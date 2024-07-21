#include "isl/unique_any.hpp"
#include "isl/detail/debug/debug.hpp"
#include "isl/lifetime.hpp"

TEST_CASE("UniqueAnyString", "[UniqueAny]")
{
    auto object = isl::UniqueAny(std::string{"Hello, World! It's a long string!"});
    REQUIRE(object.get<int>() == nullptr);

    auto object_back = object.get<std::string>();
    REQUIRE(*object_back == "Hello, World! It's a long string!");
}