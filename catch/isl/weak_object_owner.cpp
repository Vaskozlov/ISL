#include "isl/detail/debug/debug.hpp"
#include "isl/detail/object_owner.hpp"
#include "isl/lifetime.hpp"
#include "isl/memory.hpp"

TEST_CASE("WeakObjectOwnerDebugString", "[ObjectOwner]")
{
    using namespace isl;
    using namespace isl::detail;

    auto outer_reference = UniquePtr<ObjectReferenceDebug<isl::String>>{};

    {
        auto object = isl::String{"Hello, world!"};
        auto weak_owner = WeakObjectOwnerDebug{object};
        auto inner_reference = weak_owner.createReference();
        outer_reference = makeUnique<ObjectReferenceDebug<isl::String>>(inner_reference);

        REQUIRE(*inner_reference == "Hello, world!");
    }

    REQUIRE_THROWS(*(*outer_reference));
}

TEST_CASE("WeakObjectOwnerReleaseString", "[ObjectOwner]")
{
    using namespace isl;
    using namespace isl::detail;

    auto outer_reference = UniquePtr<ObjectReferenceRelease<isl::String>>{};

    {
        auto object = isl::String{"Hello, world!"};
        auto weak_owner = WeakObjectOwnerRelease{object};
        auto inner_reference = weak_owner.createReference();

        outer_reference = makeUnique<ObjectReferenceRelease<isl::String>>(inner_reference);

        REQUIRE(*inner_reference == "Hello, world!");
    }
}
