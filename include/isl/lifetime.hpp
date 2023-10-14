#ifndef ISL_PROJECT_LIFETIME_HPP
#define ISL_PROJECT_LIFETIME_HPP

#include <atomic>
#include <exception>
#include <fmt/format.h>
#include <isl/isl.hpp>
#include <isl/raii.hpp>
#include <mutex>

namespace isl::lifetime
{
    class LifetimeMonitor;

    template<typename T>
    class LifetimeProtector;

    template<typename T>
    class LifetimeObjectWithValue;

    namespace detail
    {
        class LifetimeObject
        {
        private:
            friend LifetimeMonitor;

            template<typename T>
            friend class LifetimeProtector;

            template<typename T>
            friend class LifetimeObjectWithValue;

            static inline std::mutex AppendToCreatedObjectMutex;                 // NOLINT
            static inline constinit std::atomic<Id> CurrentId = 1;               // NOLINT
            static inline Vector<std::unique_ptr<LifetimeObject>> CreatedObjects;// NOLINT

            Id uniqueId{CurrentId.fetch_add(1U, std::memory_order_relaxed)};
            Id weakId{uniqueId};
            bool moved{false};
            bool deleted{false};

            LifetimeObject()
            {
                const auto lock = std::scoped_lock{AppendToCreatedObjectMutex};
                CreatedObjects.emplace_back(this);
            }

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

#endif /* ISL_PROJECT_LIFETIME_HPP */
