#include "isl/unique_any.hpp"
#include "isl/detail/debug/debug.hpp"
#include "isl/lifetime.hpp"

TEST_CASE("UniqueAnyInt", "[UniqueAny]")
{
    auto object = isl::makeAny<int>(42);
    REQUIRE_THROWS(isl::get<std::string>(object));

    REQUIRE(*isl::observe<int>(object) == 42);
    REQUIRE(isl::get<int>(object) == 42);

    REQUIRE_THROWS(isl::get<int>(object));
}

TEST_CASE("UniqueAnyString", "[UniqueAny]")
{
    auto stored_string = std::string{"Hello, World! It's a long string!"};
    auto object = isl::makeAny<std::string>(stored_string);
    REQUIRE_THROWS(isl::get<int>(object));

    REQUIRE(*isl::observe<std::string>(object) == stored_string);

    auto object_back = isl::get<std::string>(object);
    REQUIRE(object_back == "Hello, World! It's a long string!");

    REQUIRE_THROWS(isl::get<std::string>(object));
}

TEST_CASE("UniqueAnyUniquePtr", "[UniqueAny]")
{
    using StrUniquePtr = std::unique_ptr<std::string>;

    auto stored_string = std::string{"Hello, World! It's a long string!"};
    auto object = isl::makeAny<StrUniquePtr>(std::make_unique<std::string>(stored_string));

    REQUIRE_THROWS(isl::get<std::string>(object));
    REQUIRE(*(*isl::observe<StrUniquePtr>(object)) == stored_string);

    auto object_back = isl::get<StrUniquePtr>(object);
    REQUIRE(*object_back == "Hello, World! It's a long string!");

    REQUIRE_THROWS(isl::get<std::string>(object));
}