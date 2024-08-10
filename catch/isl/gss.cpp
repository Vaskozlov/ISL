#include <algorithm>
#include <iostream>
#include <isl/detail/debug/debug.hpp>
#include <isl/gss.hpp>

TEST_CASE("GSS", "[GSS]")
{
    auto gss = isl::GSS<int>{};
    auto [first_node, first_inserted] = gss.emplace(0, nullptr, 1);
    auto [second_node, second_inserted] = gss.emplace(1, first_node, 2);

    REQUIRE(first_inserted);
    REQUIRE(second_inserted);

    REQUIRE(first_node->previous.empty());
    REQUIRE(second_node->previous.front() == first_node);

    auto [first_dup_node, first_dup_inserted] = gss.emplace(0, nullptr, 1);

    REQUIRE_FALSE(first_dup_inserted);
    REQUIRE(first_node == first_dup_node);

    gss.erase(second_node);
    auto [second_dup_node, second_dup_inserted] = gss.emplace(1, first_node, 2);

    REQUIRE(second_dup_inserted);
}
