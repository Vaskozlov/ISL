#ifndef ISL_PROJECT_LIFETIME_HPP
#define ISL_PROJECT_LIFETIME_HPP

#include <atomic>
#include <isl/id_generator.hpp>
#include <isl/isl.hpp>
#include <mutex>

namespace isl::lifetime
{
    class LifetimeMonitor;

    namespace detail
    {
        class LifetimeObject final
        {
        private:
            friend LifetimeMonitor;

            static IdGenerator<Id> LifetimeObjectIdGenerator; // NOLINT
            static std::mutex ObjectCreationLock;             // NOLINT

            Id uniqueId{LifetimeObjectIdGenerator.next()};
            Id weakId{uniqueId};
            bool moved{false};
            bool deleted{false};

            LifetimeObject();

        public:
            LifetimeObject(LifetimeObject &&) = delete;
            LifetimeObject(const LifetimeObject &) = delete;

            ~LifetimeObject() = default;

            auto operator=(LifetimeObject &&) -> void = delete;
            auto operator=(const LifetimeObject &) -> void = delete;

            friend auto completeMove(LifetimeObject &from, LifetimeObject &to) -> void;
            friend auto completeCopy(const LifetimeObject &from, LifetimeObject &to) -> void;

            ISL_DECL auto getUniqueId() const noexcept -> Id
            {
                return uniqueId;
            }

            ISL_DECL auto getWeakId() const noexcept -> Id
            {
                return weakId;
            }

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

        extern std::vector<std::unique_ptr<LifetimeObject>> CreatedObjects; // NOLINT
    } // namespace detail

    class LifetimeMonitor
    {
        detail::LifetimeObject *lifetimeObject{};

    public:
        LifetimeMonitor();

        LifetimeMonitor(const LifetimeMonitor &other);
        LifetimeMonitor(LifetimeMonitor &&other) noexcept;
        ~LifetimeMonitor();

        auto operator=(LifetimeMonitor &&other) noexcept -> LifetimeMonitor &;
        auto operator=(const LifetimeMonitor &other) -> LifetimeMonitor &;

        [[nodiscard]] auto getUniqueId() const noexcept -> Id
        {
            return lifetimeObject->getUniqueId();
        }

        [[nodiscard]] auto getWeakId() const noexcept -> Id
        {
            return lifetimeObject->getWeakId();
        }
    };
} // namespace isl::lifetime

#endif /* ISL_PROJECT_LIFETIME_HPP */
