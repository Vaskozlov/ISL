#include "isl/detail/object_owner.hpp"
#include "isl/detail/debug/debug.hpp"
#include "isl/lifetime.hpp"
#include "isl/memory.hpp"

TEST_CASE("ObjectOwnerDebugString", "[ObjectOwner]")
{
    using namespace isl;
    using namespace isl::detail;

    auto outer_reference = UniquePtr<ObjectReferenceDebug<std::string>>{};

    {
        auto object = ObjectOwnerDebug<std::string>{"Hello, world!"};
        auto object_with_the_same_lifetime = std::string{};
        auto object_copy = object;
        auto inner_reference = object.createReference();
        auto reference_with_same_lifetime =
            inner_reference.createReferenceWithSameLifetime(object_with_the_same_lifetime);

        outer_reference = makeUnique<ObjectReferenceDebug<std::string>>(inner_reference);

        REQUIRE(*inner_reference == "Hello, world!");

        auto moved_object = ObjectOwnerDebug<std::string>{std::move(object)};
    }

    REQUIRE_THROWS(*(*outer_reference));
}

TEST_CASE("ObjectOwnerReleaseString", "[ObjectOwner]")
{
    using namespace isl;
    using namespace isl::detail;

    auto outer_reference = UniquePtr<ObjectReferenceRelease<std::string>>{};

    {
        auto object = ObjectOwnerRelease<std::string>("Hello, world!");
        auto inner_reference = object.createReference();
        outer_reference = makeUnique<ObjectReferenceRelease<std::string>>(inner_reference);

        REQUIRE(*inner_reference == "Hello, world!");
    }
}

TEST_CASE("ObjectOwnerDebugLifetime", "[ObjectOwner]")
{
    using namespace isl;
    using namespace isl::detail;

    auto outer_reference = UniquePtr<ObjectReferenceDebug<lifetime::LifetimeMonitor>>{};

    {
        auto object = ObjectOwnerDebug<lifetime::LifetimeMonitor>{};
        auto object_copy = object;
        auto inner_reference = object.createReference();
        outer_reference =
            makeUnique<ObjectReferenceDebug<lifetime::LifetimeMonitor>>(inner_reference);

        REQUIRE((*outer_reference.get())->getWeakId() == 1);

        auto moved_object = ObjectOwnerDebug<lifetime::LifetimeMonitor>{std::move(object)};

        REQUIRE(moved_object.getWeakId() == 1);
    }

    REQUIRE_THROWS(*(*outer_reference));
}
