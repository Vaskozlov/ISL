#include <algorithm>
#include <iostream>
#include <isl/detail/debug/debug.hpp>
#include <isl/weak_stack.hpp>

TEST_CASE("WeakStackOfInt", "[WeakStack]")
{
    auto list = isl::WeakStack<int>{};
    list.emplace(10);
    list.emplace(20);

    REQUIRE(std::ranges::equal(list, std::vector<int>{20, 10}));

    auto list_alternative = list;
    list_alternative.emplace(30);
    list.emplace(40);

    REQUIRE(std::ranges::equal(list, std::vector<int>{40, 20, 10}));
    REQUIRE(std::ranges::equal(list_alternative, std::vector<int>{30, 20, 10}));

    list.pop();
    list.pop();
    list.pop();

    REQUIRE(std::ranges::equal(list, std::vector<int>{}));

    list_alternative.pop();
    REQUIRE(std::ranges::equal(list_alternative, std::vector<int>{20, 10}));
}
