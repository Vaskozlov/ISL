#include <isl/const_string.hpp>
#include <isl/detail/debug/debug.hpp>

template<isl::ConstString STRING>
auto testConstString(isl::string_view input) -> bool
{
    DEBUG_VAR test_string = input;

    REQUIRE(STRING.empty() == test_string.empty());
    REQUIRE(STRING.size() == test_string.size());
    REQUIRE(STRING == test_string);

    REQUIRE(std::equal(STRING.cbegin(), STRING.cend(), test_string.cbegin(), test_string.cend()));
    REQUIRE(std::equal(STRING.begin(), STRING.end(), test_string.begin()));

    return true;
}

TEST_CASE("ConstStringEmptyInput", "[ConstString]")
{
    REQUIRE(testConstString<"">(""));
}

TEST_CASE("ConstStringSingleBasicInput", "[ConstString]")
{
    REQUIRE(testConstString<"Hello, World!">("Hello, World!"));
}
