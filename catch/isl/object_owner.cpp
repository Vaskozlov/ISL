#include "isl/detail/object_owner.hpp"
#include "isl/detail/debug/debug.hpp"
#include "isl/lifetime.hpp"

TEST_CASE("ObjectOwnerDebugString", "[ObjectOwner]")
{
    std::unique_ptr<isl::detail::ObjectReferenceDebug<std::string>> outer_reference;

    {
        auto object = isl::detail::ObjectOwnerDebug<std::string>{"Hello, world!"};
        auto object_with_the_same_lifetime = std::string{};
        auto object_copy = object;
        auto inner_reference = object.createReference();
        auto reference_with_same_lifetime =
            inner_reference.createReferenceWithSameLifetime(object_with_the_same_lifetime);

        outer_reference =
            std::make_unique<isl::detail::ObjectReferenceDebug<std::string>>(inner_reference);

        REQUIRE(*inner_reference == "Hello, world!");

        auto moved_object = isl::detail::ObjectOwnerDebug<std::string>{std::move(object)};
    }

    REQUIRE_THROWS(*(*outer_reference));
}

TEST_CASE("ObjectOwnerReleaseString", "[ObjectOwner]")
{
    std::unique_ptr<isl::detail::ObjectReferenceRelease<std::string>> outer_reference;

    {
        auto object = isl::detail::ObjectOwnerRelease<std::string>("Hello, world!");
        auto inner_reference = object.createReference();
        outer_reference =
            std::make_unique<isl::detail::ObjectReferenceRelease<std::string>>(inner_reference);

        REQUIRE(*inner_reference == "Hello, world!");
    }
}

TEST_CASE("ObjectOwnerDebugLifetime", "[ObjectOwner]")
{
    std::unique_ptr<isl::detail::ObjectReferenceDebug<isl::lifetime::LifetimeMonitor>>
        outer_reference;

    {
        auto object = isl::detail::ObjectOwnerDebug<isl::lifetime::LifetimeMonitor>{};
        auto object_copy = object;
        auto inner_reference = object.createReference();
        outer_reference =
            std::make_unique<isl::detail::ObjectReferenceDebug<isl::lifetime::LifetimeMonitor>>(
                inner_reference);

        REQUIRE((*outer_reference.get())->getWeakId() == 1);

        auto moved_object =
            isl::detail::ObjectOwnerDebug<isl::lifetime::LifetimeMonitor>{std::move(object)};

        REQUIRE(moved_object.getWeakId() == 1);
    }

    REQUIRE_THROWS(*(*outer_reference));
}
