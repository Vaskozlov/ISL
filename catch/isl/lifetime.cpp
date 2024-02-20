#include <fmt/format.h>
#include <isl/lifetime.hpp>

namespace isl::lifetime
{
    auto detail::completeCopy(const LifetimeObject &from, LifetimeObject &to) -> void
    {
        to.moved = from.moved;
        to.deleted = from.deleted;
    }

    auto detail::completeMove(LifetimeObject &from, LifetimeObject &to) -> void
    {
        to.weakId = from.weakId;
        to.moved = from.moved;
        to.deleted = from.deleted;

        from.moved = true;
    }

    detail::LifetimeObject::LifetimeObject()
    {
        const auto lock = std::scoped_lock{LockForObjectCreation};
        CreatedObjects.emplace_back(this);
    }

    LifetimeMonitor::LifetimeMonitor()
      : lifetimeObject{new detail::LifetimeObject}
    {
        fmt::println(
            "Constructor: object ({}, {}) constructed", lifetimeObject->uniqueId,
            lifetimeObject->weakId);
    }

    LifetimeMonitor::LifetimeMonitor(const LifetimeMonitor &other)
      : lifetimeObject{new detail::LifetimeObject}
    {
        if (other.lifetimeObject->deleted) {
            fmt::println(
                "Copy constructor: unable to copy from a deleted object ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId);
        } else if (other.lifetimeObject->moved) {
            fmt::println(
                "Copy constructor: unable to copy from a moved object ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId);
        } else {
            detail::completeCopy(*other.lifetimeObject, *lifetimeObject);
            fmt::println(
                "Copy constructor: object ({}, {}) has been copied from ({}, {})",
                lifetimeObject->uniqueId, lifetimeObject->weakId, other.lifetimeObject->uniqueId,
                other.lifetimeObject->weakId);
        }
    }

    LifetimeMonitor::LifetimeMonitor(LifetimeMonitor &&other) noexcept
      : lifetimeObject{new detail::LifetimeObject}
    {
        if (other.lifetimeObject->deleted) {
            fmt::println(
                "Move constructor: unable to move from a deleted object ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId);
        } else {
            detail::completeMove(*other.lifetimeObject, *lifetimeObject);
            fmt::println(
                "Move constructor: object ({}, {}) has been moved to ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId,
                lifetimeObject->uniqueId, lifetimeObject->weakId);
        }
    }

    LifetimeMonitor::~LifetimeMonitor()
    {
        if (!lifetimeObject->deleteObject()) {
            fmt::println(
                "Destructor: doube delete on object ({}, {})", lifetimeObject->uniqueId,
                lifetimeObject->weakId);
        } else {
            fmt::println(
                "Destructor: object ({}, {}) deleted", lifetimeObject->uniqueId,
                lifetimeObject->weakId);
        }
    }

    auto LifetimeMonitor::operator=(LifetimeMonitor &&other) noexcept -> LifetimeMonitor &
    {
        if (other.lifetimeObject->deleted) {
            fmt::println(
                "Move assign: unable to move from a deleted object ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId);
        } else {
            detail::completeMove(*other.lifetimeObject, *lifetimeObject);
            fmt::println(
                "Move assign: object ({}, {}) has been moved to ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId,
                lifetimeObject->uniqueId, lifetimeObject->weakId);
        }
        return *this;
    }

    auto LifetimeMonitor::operator=(const LifetimeMonitor &other) -> LifetimeMonitor &
    {
        if (this == &other) {
            return *this;
        }

        if (other.lifetimeObject->deleted) {
            fmt::println(
                "Copy assign: unable to move from a deleted object ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId);
        } else {
            detail::completeCopy(*other.lifetimeObject, *lifetimeObject);
            fmt::println(
                "Copy assign: object ({}, {}) has been moved to ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId,
                lifetimeObject->uniqueId, lifetimeObject->weakId);
        }
        return *this;
    }
}// namespace isl::lifetime
