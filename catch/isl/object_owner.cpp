#include "isl/detail/object_owner.hpp"
#include "isl/detail/debug/debug.hpp"
#include "isl/lifetime.hpp"
#include "isl/memory.hpp"

TEST_CASE("ObjectOwnerDebugString", "[ObjectOwner]")
{
    using namespace isl;
    using namespace isl::detail;

    auto outer_reference = UniquePtr<ObjectReferenceDebug<isl::String>>{};

    {
        auto object = ObjectOwnerDebug<isl::String>{"Hello, world!"};
        auto object_with_the_same_lifetime = isl::String{};
        auto object_copy = object;
        auto inner_reference = object.createReference();
        auto reference_with_same_lifetime =
            inner_reference.createReferenceWithSameLifetime(object_with_the_same_lifetime);

        outer_reference = makeUnique<ObjectReferenceDebug<isl::String>>(inner_reference);

        REQUIRE(*inner_reference == "Hello, world!");

        auto moved_object = ObjectOwnerDebug<isl::String>{std::move(object)};
    }

    REQUIRE_THROWS(*(*outer_reference));
}

TEST_CASE("ObjectOwnerReleaseString", "[ObjectOwner]")
{
    using namespace isl;
    using namespace isl::detail;

    auto outer_reference = UniquePtr<ObjectReferenceRelease<isl::String>>{};

    {
        auto object = ObjectOwnerRelease<isl::String>("Hello, world!");
        auto inner_reference = object.createReference();
        outer_reference = makeUnique<ObjectReferenceRelease<isl::String>>(inner_reference);

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

        auto weak_id = (*outer_reference.get())->getWeakId();
        auto moved_object = ObjectOwnerDebug<lifetime::LifetimeMonitor>{std::move(object)};

        REQUIRE(moved_object.getWeakId() == weak_id);
    }

    REQUIRE_THROWS(*(*outer_reference));
}
