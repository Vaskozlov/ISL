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

TEST_CASE("SmallVectorOfIntsClean", "[SmallVector]")
{
    {
        auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4};

        vector.clean();
        REQUIRE(vector.empty());
    }

    {
        auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4, 5};

        vector.clean();
        REQUIRE(vector.empty());
    }
}


TEST_CASE("SmallVectorOfIntsCopy", "[SmallVector]")
{
    auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4};

    {
        auto small_vector_copy = vector;
        REQUIRE(std::ranges::equal(vector, std::vector{1, 2, 3, 4}));
        REQUIRE(std::ranges::equal(vector, small_vector_copy));
    }

    {
        auto small_vector_copy = isl::SmallVector<int, 4>{};
        small_vector_copy = vector;

        REQUIRE(std::ranges::equal(vector, std::vector{1, 2, 3, 4}));
        REQUIRE(std::ranges::equal(vector, small_vector_copy));
    }

    {
        auto empty_vector = isl::SmallVector<int, 4>{};
        auto small_vector_copy = vector;
        small_vector_copy = empty_vector;

        REQUIRE(small_vector_copy.empty());
        REQUIRE(std::ranges::equal(small_vector_copy, std::vector<int>{}));
    }

    vector.emplace_back(5);

    {
        auto small_vector_copy = vector;
        REQUIRE(std::ranges::equal(vector, std::vector{1, 2, 3, 4, 5}));
        REQUIRE(std::ranges::equal(vector, small_vector_copy));
    }

    {
        auto small_vector_copy = isl::SmallVector<int, 4>{};
        small_vector_copy = vector;

        REQUIRE(std::ranges::equal(vector, std::vector{1, 2, 3, 4, 5}));
        REQUIRE(std::ranges::equal(vector, small_vector_copy));
    }

    {
        auto empty_vector = isl::SmallVector<int, 4>{};
        auto small_vector_copy = vector;
        small_vector_copy = empty_vector;

        REQUIRE(small_vector_copy.empty());
        REQUIRE(std::ranges::equal(small_vector_copy, std::vector<int>{}));
    }
}

TEST_CASE("SmallVectorOfIntsMove", "[SmallVector]")
{
    {
        auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4};

        auto small_vector_copy = std::move(vector);
        REQUIRE(std::ranges::equal(small_vector_copy, std::vector{1, 2, 3, 4}));
    }

    {
        auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4};

        auto small_vector_copy = isl::SmallVector<int, 4>{};
        small_vector_copy = std::move(vector);

        REQUIRE(std::ranges::equal(small_vector_copy, std::vector{1, 2, 3, 4}));
    }

    {
        auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4};

        auto empty_vector = isl::SmallVector<int, 4>{};
        auto small_vector_copy = std::move(vector);
        small_vector_copy = std::move(empty_vector);

        REQUIRE(small_vector_copy.empty());
    }

    {
        auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4, 5};

        auto small_vector_copy = std::move(vector);
        REQUIRE(std::ranges::equal(small_vector_copy, std::vector{1, 2, 3, 4, 5}));
    }

    {
        auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4, 5};

        auto small_vector_copy = isl::SmallVector<int, 4>{};
        small_vector_copy = std::move(vector);

        REQUIRE(std::ranges::equal(small_vector_copy, std::vector{1, 2, 3, 4, 5}));
    }

    {
        auto vector = isl::SmallVector<int, 4>{1, 2, 3, 4, 5};

        auto empty_vector = isl::SmallVector<int, 4>{};
        auto small_vector_copy = std::move(vector);
        small_vector_copy = std::move(empty_vector);

        REQUIRE(small_vector_copy.empty());
    }
}