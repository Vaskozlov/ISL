#include <isl/detail/debug/debug.hpp>
#include <isl/dynamic_forward_list.hpp>
#include <isl/lifetime.hpp>

namespace
{
    struct DynamicForwardListNode : isl::DynamicForwardListNode
    {
        int value;

        explicit DynamicForwardListNode(int initial_value)
          : value{initial_value}
        {}
    };
}// namespace

TEST_CASE("DynamicForwardList", "[ForwardList]")
{
    auto forward_list = isl::DynamicForwardList<DynamicForwardListNode>{};
    auto transformer = [&forward_list] {
        return std::views::transform(forward_list, [](const DynamicForwardListNode *ptr) {
            return ptr->value;
        });
    };

    forward_list.emplaceFront(10);
    forward_list.emplaceFront(20);

    REQUIRE(std::ranges::equal(transformer(), std::vector<int>{20, 10}));

    forward_list.popFront();

    REQUIRE(std::ranges::equal(transformer(), std::vector<int>{10}));

    forward_list.popFront();

    REQUIRE(std::ranges::equal(transformer(), std::vector<int>{}));
}
