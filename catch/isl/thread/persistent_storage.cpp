#include <iostream>
#include <isl/detail/debug/debug.hpp>
#include <isl/thread/persistent_storage.hpp>

// NOLINTBEGIN

TEST_CASE("PriorityLazyListBasicTest", "[LazyList]")
{
    auto list = isl::thread::PersistentStorage<int>{};
    auto second_list = isl::thread::PersistentStorage<int>{};

    list.emplaceFront(1);
    list.emplaceFront(2);
    list.emplaceFront(0);
    list.emplaceFront(5);

    REQUIRE(std::equal(list.begin(), list.end(), std::vector{5, 0, 2, 1}.begin()));

    auto it = list.emplaceFront(100);
    REQUIRE(std::equal(list.begin(), list.end(), std::vector{100, 5, 0, 2, 1}.begin()));

    list.erase(it);
    REQUIRE(std::equal(list.begin(), list.end(), std::vector{5, 0, 2, 1}.begin()));

    it = list.emplaceFront(100);
    REQUIRE(std::equal(list.begin(), list.end(), std::vector{100, 5, 0, 2, 1}.begin()));

    auto released = list.release(it);
    REQUIRE(std::equal(list.begin(), list.end(), std::vector{5, 0, 2, 1}.begin()));

    second_list.insertFront(std::move(released));
    REQUIRE(std::equal(second_list.begin(), second_list.end(), std::vector{100}.begin()));

    list.clear();
    REQUIRE(list.empty());
}

// NOLINTEND
