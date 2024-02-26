module;

#include <isl/detail/defines.hpp>
#include <isl/std.hpp>

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
}// namespace isl::lifetime
