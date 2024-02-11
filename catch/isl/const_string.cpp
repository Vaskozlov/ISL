#include <isl/const_string.hpp>
#include <isl/detail/debug/debug.hpp>

template<isl::ConstString String>
auto testConstString(isl::string_view input) -> bool
{
    DEBUG_VAR test_string = input;

    REQUIRE(String.empty() == test_string.empty());
    REQUIRE(String.size() == test_string.size());
    REQUIRE(String == test_string);

    REQUIRE(std::equal(String.cbegin(), String.cend(), test_string.cbegin(), test_string.cend()));
    REQUIRE(std::equal(String.begin(), String.end(), test_string.begin()));

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
