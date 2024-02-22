module;

#include <atomic>
#include <isl/detail/defines.hpp>
#include <memory>
#include <mutex>
#include <print>

export module isl:lifetime;

export import :types;

export namespace isl::lifetime
{
    class LifetimeMonitor;

    namespace detail
    {
        class LifetimeObject
        {
        private:
            friend LifetimeMonitor;

            static inline constinit std::atomic<Id> CurrentId = 1;// NOLINT
            static inline std::mutex LockForObjectCreation;       // NOLINT

            static inline auto autogetCreatedObjects() -> Vector<std::unique_ptr<LifetimeObject>> &
            {
                static Vector<std::unique_ptr<LifetimeObject>> CreatedObjects;// NOLINT
                return CreatedObjects;
            }

            Id uniqueId{CurrentId.fetch_add(1U, std::memory_order_relaxed)};
            Id weakId{uniqueId};
            bool moved{false};
            bool deleted{false};

            LifetimeObject();

        public:
            LifetimeObject(LifetimeObject &&) = delete;
            LifetimeObject(const LifetimeObject &) = delete;

            virtual ~LifetimeObject() = default;

            auto operator=(LifetimeObject &&) -> void = delete;
            auto operator=(const LifetimeObject &) -> void = delete;

            friend auto completeMove(LifetimeObject &from, LifetimeObject &to) -> void;
            friend auto completeCopy(const LifetimeObject &from, LifetimeObject &to) -> void;

            ISL_DECL auto canBeUsed() const noexcept -> bool
            {
                return !moved && !deleted;
            }

            ISL_DECL auto deleteObject() noexcept -> bool
            {
                if (deleted) {
                    return false;
                }

                deleted = true;
                return true;
            }
        };

        auto completeCopy(const LifetimeObject &from, LifetimeObject &to) -> void
        {
            to.moved = from.moved;
            to.deleted = from.deleted;
        }

        auto completeMove(LifetimeObject &from, LifetimeObject &to) -> void
        {
            to.weakId = from.weakId;
            to.moved = from.moved;
            to.deleted = from.deleted;

            from.moved = true;
        }
    }// namespace detail

    class LifetimeMonitor
    {
        detail::LifetimeObject *lifetimeObject{nullptr};

    public:
        LifetimeMonitor();

        LifetimeMonitor(const LifetimeMonitor &other);
        LifetimeMonitor(LifetimeMonitor &&other) noexcept;
        ~LifetimeMonitor();

        auto operator=(LifetimeMonitor &&other) noexcept -> LifetimeMonitor &;
        auto operator=(const LifetimeMonitor &other) -> LifetimeMonitor &;
    };

    detail::LifetimeObject::LifetimeObject()
    {
        const auto lock = std::scoped_lock{LockForObjectCreation};
        autogetCreatedObjects().emplace_back(this);
    }

    LifetimeMonitor::LifetimeMonitor()
      : lifetimeObject{new detail::LifetimeObject}
    {
        std::println(
            "Constructor: object ({}, {}) constructed", lifetimeObject->uniqueId,
            lifetimeObject->weakId);
    }

    LifetimeMonitor::LifetimeMonitor(const LifetimeMonitor &other)
      : lifetimeObject{new detail::LifetimeObject}
    {
        if (other.lifetimeObject->deleted) {
            std::println(
                "Copy constructor: unable to copy from a deleted object ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId);
        } else if (other.lifetimeObject->moved) {
            std::println(
                "Copy constructor: unable to copy from a moved object ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId);
        } else {
            detail::completeCopy(*other.lifetimeObject, *lifetimeObject);
            std::println(
                "Copy constructor: object ({}, {}) has been copied from ({}, {})",
                lifetimeObject->uniqueId, lifetimeObject->weakId, other.lifetimeObject->uniqueId,
                other.lifetimeObject->weakId);
        }
    }

    LifetimeMonitor::LifetimeMonitor(LifetimeMonitor &&other) noexcept
      : lifetimeObject{new detail::LifetimeObject}
    {
        if (other.lifetimeObject->deleted) {
            std::println(
                "Move constructor: unable to move from a deleted object ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId);
        } else {
            detail::completeMove(*other.lifetimeObject, *lifetimeObject);
            std::println(
                "Move constructor: object ({}, {}) has been moved to ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId,
                lifetimeObject->uniqueId, lifetimeObject->weakId);
        }
    }

    LifetimeMonitor::~LifetimeMonitor()
    {
        if (!lifetimeObject->deleteObject()) {
            std::println(
                "Destructor: doube delete on object ({}, {})", lifetimeObject->uniqueId,
                lifetimeObject->weakId);
        } else {
            std::println(
                "Destructor: object ({}, {}) deleted", lifetimeObject->uniqueId,
                lifetimeObject->weakId);
        }
    }

    auto LifetimeMonitor::operator=(LifetimeMonitor &&other) noexcept -> LifetimeMonitor &
    {
        if (other.lifetimeObject->deleted) {
            std::println(
                "Move assign: unable to move from a deleted object ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId);
        } else {
            detail::completeMove(*other.lifetimeObject, *lifetimeObject);
            std::println(
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
            std::println(
                "Copy assign: unable to move from a deleted object ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId);
        } else {
            detail::completeCopy(*other.lifetimeObject, *lifetimeObject);
            std::println(
                "Copy assign: object ({}, {}) has been moved to ({}, {})",
                other.lifetimeObject->uniqueId, other.lifetimeObject->weakId,
                lifetimeObject->uniqueId, lifetimeObject->weakId);
        }
        return *this;
    }
}// namespace isl::lifetime
