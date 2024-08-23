#include "isl/detail/debug/debug.hpp"
#include "isl/detail/object_owner.hpp"
#include "isl/lifetime.hpp"
#include "isl/memory.hpp"

TEST_CASE("WeakObjectOwnerDebugString", "[ObjectOwner]")
{
    using namespace isl;
    using namespace isl::detail;

    auto outer_reference = std::unique_ptr<ObjectReferenceDebug<std::string>>{};

    {
        auto object = std::string{"Hello, world!"};
        auto weak_owner = WeakObjectOwnerDebug{object};
        auto inner_reference = weak_owner.createReference();
        outer_reference = std::make_unique<ObjectReferenceDebug<std::string>>(inner_reference);

        REQUIRE(*inner_reference == "Hello, world!");
    }

    REQUIRE_THROWS(*(*outer_reference));
}

TEST_CASE("WeakObjectOwnerReleaseString", "[ObjectOwner]")
{
    using namespace isl;
    using namespace isl::detail;

    auto outer_reference = std::unique_ptr<ObjectReferenceRelease<std::string>>{};

    {
        auto object = std::string{"Hello, world!"};
        auto weak_owner = WeakObjectOwnerRelease{object};
        auto inner_reference = weak_owner.createReference();

        outer_reference = std::make_unique<ObjectReferenceRelease<std::string>>(inner_reference);

        REQUIRE(*inner_reference == "Hello, world!");
    }
}
