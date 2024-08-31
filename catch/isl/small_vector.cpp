#include "isl/small_vector.hpp"
#include "isl/detail/debug/debug.hpp"

TEST_CASE("SmallVectorOfIntsConstruct", "[SmallVector]")
{
    auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4};

    REQUIRE(std::ranges::equal(vector, std::vector{1, 2, 3, 4}));

    vector.emplace_back(5);
    vector.emplace_back(6);

    REQUIRE(std::ranges::equal(vector, std::vector{1, 2, 3, 4, 5, 6}));

    vector.pop_back();

    REQUIRE(std::ranges::equal(vector, std::vector{1, 2, 3, 4, 5}));

    for (std::size_t i = 0; i != 5; ++i) {
        vector.pop_back();
    }

    REQUIRE(std::ranges::equal(vector, std::vector<int>{}));
    REQUIRE(vector.empty());
}

TEST_CASE("SmallVectorOfIntsCleanInternalBuffer", "[SmallVector]")
{
    auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4};

    vector.clean();
    REQUIRE(vector.empty());
}

TEST_CASE("SmallVectorOfIntsCleanAllocatedBuffer", "[SmallVector]")
{
    auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4, 5};

    vector.clean();
    REQUIRE(vector.empty());
}

TEST_CASE("SmallVectorOfIntsCopyInternalBuffer", "[SmallVector]")
{
    auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4};
    auto small_vector_copy = vector;
    REQUIRE(std::ranges::equal(vector, std::vector{1, 2, 3, 4}));
    REQUIRE(std::ranges::equal(vector, small_vector_copy));
}

TEST_CASE("SmallVectorOfIntsCopyAllocatedBuffer", "[SmallVector]")
{
    auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4, 5};
    auto small_vector_copy = vector;
    REQUIRE(std::ranges::equal(vector, std::vector{1, 2, 3, 4, 5}));
    REQUIRE(std::ranges::equal(vector, small_vector_copy));
}

TEST_CASE("SmallVectorOfIntsCopyFromSmallerInternalBuffer", "[SmallVector]")
{
    auto vector = isl::SmallVector<int, 4>{1, 2, 3};
    auto small_vector_copy = isl::SmallVector<int, 4>{1, 2, 3, 4};

    small_vector_copy = vector;
    REQUIRE(std::ranges::equal(vector, std::vector{1, 2, 3}));
    REQUIRE(std::ranges::equal(vector, small_vector_copy));
}

TEST_CASE("SmallVectorOfIntsCopyFromSmallerAllocatedBuffer", "[SmallVector]")
{
    auto vector = isl::SmallVector<int, 4>{1, 2, 3};
    auto small_vector_copy = isl::SmallVector<int, 4>{1, 2, 3, 4, 5};

    small_vector_copy = vector;
    REQUIRE(std::ranges::equal(vector, std::vector{1, 2, 3}));
    REQUIRE(std::ranges::equal(vector, small_vector_copy));
}

TEST_CASE("SmallVectorOfIntsCopyAllocatedBufferFromSmallerAllocatedBuffer", "[SmallVector]")
{
    auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4, 5};
    auto small_vector_copy = isl::SmallVector<int, 4>{1, 2, 3, 4, 5, 6};

    small_vector_copy = vector;
    REQUIRE(std::ranges::equal(vector, std::vector{1, 2, 3, 4, 5}));
    REQUIRE(std::ranges::equal(vector, small_vector_copy));
}

TEST_CASE("SmallVectorOfIntsMoveConstructorInternalBuffer", "[SmallVector]") {
    auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4};

    auto small_vector_copy = std::move(vector);
    REQUIRE(std::ranges::equal(small_vector_copy, std::vector{1, 2, 3, 4}));
}

TEST_CASE("SmallVectorOfIntsMoveConstructorAllocatedBuffer", "[SmallVector]") {
    auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4, 5};

    auto small_vector_copy = std::move(vector);
    REQUIRE(std::ranges::equal(small_vector_copy, std::vector{1, 2, 3, 4, 5}));
}

TEST_CASE("SmallVectorOfIntsMoveAssignmentFromSmallerInternalBuffer", "[SmallVector]") {
    auto vector = isl::SmallVector<int, 4>{1, 2, 3};
    auto small_vector_copy = isl::SmallVector<int, 4>{1, 2, 3, 4};

    small_vector_copy = std::move(vector);
    REQUIRE(std::ranges::equal(small_vector_copy, std::vector{1, 2, 3}));
}

TEST_CASE("SmallVectorOfIntsMoveAssignmentInternalBuffer", "[SmallVector]") {
    auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4};

    auto small_vector_copy = isl::SmallVector<int, 4>{};
    small_vector_copy = std::move(vector);

    REQUIRE(std::ranges::equal(small_vector_copy, std::vector{1, 2, 3, 4}));
}

TEST_CASE("SmallVectorOfIntsMoveAssignmentAllocatedBuffer", "[SmallVector]") {
    auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4, 5};

    auto small_vector_copy = isl::SmallVector<int, 4>{};
    small_vector_copy = std::move(vector);

    REQUIRE(std::ranges::equal(small_vector_copy, std::vector{1, 2, 3, 4, 5}));
}

TEST_CASE("SmallVectorOfIntsMoveAssignmentFromSmallerAllocatedBuffer", "[SmallVector]") {
    auto vector = isl::SmallVector<int, 4>{1, 2, 3};
    auto small_vector_copy = isl::SmallVector<int, 4>{1, 2, 3, 4, 5};

    small_vector_copy = std::move(vector);
    REQUIRE(std::ranges::equal(small_vector_copy, std::vector{1, 2, 3}));
}

TEST_CASE("SmallVectorOfIntsErase", "[SmallVector]")
{
    {
        auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4};
        vector.erase(vector.begin());
        REQUIRE(std::ranges::equal(vector, std::vector{2, 3, 4}));
    }

    {
        auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4};
        vector.erase(vector.begin() + 1);
        REQUIRE(std::ranges::equal(vector, std::vector{1, 3, 4}));
    }

    {
        auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4};
        vector.erase(vector.begin() + 2);
        REQUIRE(std::ranges::equal(vector, std::vector{1, 2, 4}));
    }

    {
        auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4};
        vector.erase(vector.begin() + 3);
        REQUIRE(std::ranges::equal(vector, std::vector{1, 2, 3}));
    }
}