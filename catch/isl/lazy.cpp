#include "isl/lazy.hpp"
#include "isl/detail/debug/debug.hpp"

TEST_CASE("LazyObjectInitializedGet", "[Lazy]")
{
    DEBUG_VAR int_object = isl::toLazy(10);// NOLINT

    REQUIRE(int_object.get() == 10);
    REQUIRE(std::as_const(int_object).get() == 10);
}

TEST_CASE("LazyObjectUninitializedGet", "[Lazy]")
{
    DEBUG_VAR some_int = 42;// NOLINT
    DEBUG_VAR int_object = isl::toLazy([some_int]() {
        return some_int;
    });

    REQUIRE(int_object.get() == some_int);
    REQUIRE(std::as_const(int_object).get() == some_int);
}