#include "isl/unique_any.hpp"
#include "isl/detail/debug/debug.hpp"
#include "isl/lifetime.hpp"

TEST_CASE("UniqueAnyInt", "[UniqueAny]")
{
    auto object = isl::makeAny<int>(42);
    REQUIRE_THROWS(isl::get<isl::String>(object));

    REQUIRE(*isl::observe<int>(object) == 42);
    REQUIRE(isl::get<int>(object) == 42);

    REQUIRE_THROWS(isl::get<int>(object));
}

TEST_CASE("UniqueAnyString", "[UniqueAny]")
{
    auto stored_string = isl::String{"Hello, World! It's a long string!"};
    auto object = isl::makeAny<isl::String>(stored_string);
    REQUIRE_THROWS(isl::get<int>(object));

    REQUIRE(*isl::observe<isl::String>(object) == stored_string);

    auto object_back = isl::get<isl::String>(object);
    REQUIRE(object_back == "Hello, World! It's a long string!");

    REQUIRE_THROWS(isl::get<isl::String>(object));
}

TEST_CASE("UniqueAnyUniquePtr", "[UniqueAny]")
{
    using StrUniquePtr = std::unique_ptr<isl::String>;

    auto stored_string = isl::String{"Hello, World! It's a long string!"};
    auto object = isl::makeAny<StrUniquePtr>(std::make_unique<isl::String>(stored_string));

    REQUIRE_THROWS(isl::get<isl::String>(object));
    REQUIRE(*(*isl::observe<StrUniquePtr>(object)) == stored_string);

    auto object_back = isl::get<StrUniquePtr>(object);
    REQUIRE(*object_back == "Hello, World! It's a long string!");

    REQUIRE_THROWS(isl::get<isl::String>(object));
}