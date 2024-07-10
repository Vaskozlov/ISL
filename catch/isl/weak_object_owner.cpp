#include "isl/detail/debug/debug.hpp"
#include "isl/detail/object_owner.hpp"
#include "isl/lifetime.hpp"

TEST_CASE("WeakObjectOwnerDebugString", "[ObjectOwner]")
{
    std::unique_ptr<isl::detail::ObjectReferenceDebug<std::string>> outer_reference;

    {
        auto object = std::string{"Hello, world!"};
        auto weak_owner = isl::detail::WeakObjectOwnerDebug{object};
        auto inner_reference = weak_owner.createReference();

        outer_reference =
            std::make_unique<isl::detail::ObjectReferenceDebug<std::string>>(inner_reference);

        REQUIRE(*inner_reference == "Hello, world!");
    }

    REQUIRE_THROWS(*(*outer_reference));
}

TEST_CASE("WeakObjectOwnerReleaseString", "[ObjectOwner]")
{
    std::unique_ptr<isl::detail::ObjectReferenceRelease<std::string>> outer_reference;

    {
        auto object = std::string{"Hello, world!"};
        auto weak_owner = isl::detail::WeakObjectOwnerRelease{object};
        auto inner_reference = weak_owner.createReference();

        outer_reference =
            std::make_unique<isl::detail::ObjectReferenceRelease<std::string>>(inner_reference);

        REQUIRE(*inner_reference == "Hello, world!");
    }
}
