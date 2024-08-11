#include <algorithm>
#include <iostream>
#include <isl/detail/debug/debug.hpp>
#include <isl/gss.hpp>

auto reducer(std::vector<int> values) -> int
{
    auto result = values.front();

    for (auto elem : values | std::views::drop(1)) {
        result *= elem;
    }

    return result;
}

TEST_CASE("GSS", "[GSS]")
{
    auto first_stack = isl::GSStack<int>{};

    first_stack.push(0, 0, 1);
    first_stack.push(1, 0, 2);

    auto second_stack = first_stack.dup();
    second_stack.push(2, 0, 3);

    auto third_stack = first_stack.dup();
    third_stack.push(2, 0, 10);

    const auto &first_stack_head = first_stack.getHead();
    REQUIRE(first_stack_head.size() == 1);
    REQUIRE(first_stack_head.front()->value == 2);
    REQUIRE(first_stack_head.front()->previous.front()->value == 1);

    const auto &second_stack_head = second_stack.getHead();
    REQUIRE(second_stack_head.size() == 1);
    REQUIRE(second_stack_head.front()->value == 3);
    REQUIRE(second_stack_head.front()->previous.front()->value == 2);
    REQUIRE(second_stack_head.front()->previous.front()->previous.front()->value == 1);

    REQUIRE_THROWS(first_stack.merge(second_stack));

    first_stack.push(2, 0, 4);
    first_stack.merge(second_stack);
    first_stack.merge(third_stack);

    first_stack.push(3, 0, 5);

    REQUIRE(first_stack_head.front()->previous.size() == 3);

    auto after_merge_copy = first_stack.dup();
    after_merge_copy.push(4, 0, 6);
    first_stack.push(4, 0, 7);

    first_stack.merge(after_merge_copy);
    first_stack.reduce(2, reducer);

    REQUIRE(first_stack_head.size() == 2);
    REQUIRE(first_stack_head.front()->value == 35);
    REQUIRE(first_stack_head.back()->value == 30);

    REQUIRE(first_stack_head.front()->previous.size() == 3);
    REQUIRE(first_stack_head.back()->previous.size() == 3);

    REQUIRE(first_stack_head.front()->previous.at(0)->value == 4);
    REQUIRE(first_stack_head.front()->previous.at(1)->value == 3);
    REQUIRE(first_stack_head.front()->previous.at(2)->value == 10);
}
