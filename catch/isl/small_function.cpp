#include <functional>
#include <isl/detail/debug/debug.hpp>
#include <isl/small_function.hpp>

TEST_CASE("SmallFunctionNoArgs", "[SmallFunction]")
{
    isl::SmallFunction<int()> a{[i = 0]() mutable {
        return i++;
    }};

    auto b = a;

    for (int i = 0; i != 100; ++i) {
        REQUIRE(a() == i);
        REQUIRE(b() == i);
    }
}

TEST_CASE("SmallFunctionWithLambdaCapture", "[SmallFunction]")
{
    int x = 10;
    isl::SmallFunction<int()> a{[&x]() {
        return x * 2;
    }};

    REQUIRE(a() == 20);

    x = 20;
    REQUIRE(a() == 40);
}

TEST_CASE("SmallFunctionWithArgsNoArgs", "[SmallFunction]")
{
    const isl::SmallFunction<int(int, int)> a{[i = 0](const int a, const int b) mutable {
        return a * b + i++;
    }};

    const isl::SmallFunction<int(int, int)> b = a;

    for (int i = 0; i != 100; ++i) {
        REQUIRE(a(i, i) == i * i + i);
        REQUIRE(b(i, i) == i * i + i);
    }
}

TEST_CASE("SmallFunctionWithDifferentReturnType", "[SmallFunction]")
{
    isl::SmallFunction<std::string(int)> a{[](int i) {
        return std::to_string(i);
    }};

    for (int i = 0; i != 100; ++i) {
        REQUIRE(a(i) == std::to_string(i));
    }
}

TEST_CASE("SmallFunctionWithComplexCapture", "[SmallFunction]")
{
    const isl::SmallFunction<int(int)> a{[vec = std::vector<int>{}](const int i) mutable {
        vec.push_back(i);
        return static_cast<int>(vec.size());
    }};

    auto b = a;

    for (int i = 0; i != 100; ++i) {
        REQUIRE(a(i) == i + 1);
        REQUIRE(b(i) == i + 1);
    }

    auto c = std::move(b);
    REQUIRE(c(100) == 101);

    b = std::move(c);
    REQUIRE(b(101) == 102);
}

TEST_CASE("SmallFunctionWithVoidReturnType", "[SmallFunction]")
{
    int x = 0;
    const isl::SmallFunction<void(int)> a{[&x](const int i) {
        x += i;
    }};

    for (int i = 0; i != 100; ++i) {
        a(i);
        REQUIRE(x == (i * (i + 1)) / 2);
    }
}

TEST_CASE("SmallFunctionWithExceptionHandling", "[SmallFunction]")
{
    const isl::SmallFunction<int()> a{[]() -> int {
        throw std::runtime_error("Test exception");
    }};

    REQUIRE_THROWS_AS(a(), std::runtime_error);
}