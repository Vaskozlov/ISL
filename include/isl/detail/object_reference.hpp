#ifndef ISL_PROJECT_OBJECT_REFERENCE_HPP
#define ISL_PROJECT_OBJECT_REFERENCE_HPP

#include <isl/detail/defines.hpp>
#include <memory>
#include <stdexcept>

namespace isl::detail
{
    template<typename T>
    class ObjectReferenceDebug
    {
    private:
        T *object{nullptr};
        std::shared_ptr<bool> isObjectDestroyed;

    public:
        ObjectReferenceDebug(const ObjectReferenceDebug &) = default;
        ObjectReferenceDebug(ObjectReferenceDebug &&) noexcept = default;

        ObjectReferenceDebug(T &original_object, std::shared_ptr<bool> is_object_destroyed)
          : object{&original_object}
          , isObjectDestroyed{std::move(is_object_destroyed)}
        {}

        ~ObjectReferenceDebug() = default;

        auto operator=(ObjectReferenceDebug &&) -> void = delete;
        auto operator=(const ObjectReferenceDebug &) -> void = delete;

        auto operator->() ISL_LIFETIMEBOUND->T *
        {
            if (*isObjectDestroyed) {
                throw std::logic_error("An attempt to use dead reference");
            }

            return object;
        }

        auto operator->() const ISL_LIFETIMEBOUND->const T *
        {
            if (*isObjectDestroyed) {
                throw std::logic_error("An attempt to use dead reference");
            }

            return object;
        }

        auto operator*() ISL_LIFETIMEBOUND->T &
        {
            if (*isObjectDestroyed) {
                throw std::logic_error("An attempt to use dead reference");
            }

            return *object;
        }

        auto operator*() const ISL_LIFETIMEBOUND->const T &
        {
            if (*isObjectDestroyed) {
                throw std::logic_error("An attempt to use dead reference");
            }

            return *object;
        }

        template<typename U>
        [[nodiscard]] auto createReferenceWithSameLifetime(U &value) const ISL_LIFETIMEBOUND
            -> ObjectReferenceDebug<U>
        {
            return ObjectReferenceDebug<U>{value, isObjectDestroyed};
        }

        ISL_DECL auto getPtr() noexcept ISL_LIFETIMEBOUND -> T *
        {
            return object;
        }

        ISL_DECL auto getPtr() const noexcept ISL_LIFETIMEBOUND -> const T *
        {
            return object;
        }
    };

    template<typename T>
    class ISL_TRIVIAL_ABI ObjectReferenceRelease
    {
    private:
        T *object{nullptr};

    public:
        ObjectReferenceRelease(const ObjectReferenceRelease &) = default;
        ObjectReferenceRelease(ObjectReferenceRelease &&) noexcept = default;

        explicit constexpr ObjectReferenceRelease(T &original_object)
          : object{&original_object}
        {}

        ~ObjectReferenceRelease() = default;

        auto operator=(ObjectReferenceRelease &&) -> void = delete;
        auto operator=(const ObjectReferenceRelease &) -> void = delete;

        ISL_DECL auto operator->() noexcept ISL_LIFETIMEBOUND->T *
        {
            return object;
        }

        ISL_DECL auto operator->() const noexcept ISL_LIFETIMEBOUND->const T *
        {
            return object;
        }

        ISL_DECL auto operator*() noexcept ISL_LIFETIMEBOUND->T &
        {
            return *object;
        }

        ISL_DECL auto operator*() const noexcept ISL_LIFETIMEBOUND->const T &
        {
            return *object;
        }

        template<typename U>
        ISL_DECL auto createReferenceWithSameLifetime(U &value) const noexcept ISL_LIFETIMEBOUND
            -> ObjectReferenceRelease<U>
        {
            return ObjectReferenceRelease<U>{value};
        }

        ISL_DECL auto getPtr() noexcept ISL_LIFETIMEBOUND -> T *
        {
            return object;
        }

        ISL_DECL auto getPtr() const noexcept ISL_LIFETIMEBOUND -> const T *
        {
            return object;
        }
    };
}// namespace isl::detail

namespace isl
{
#ifdef ISL_DEBUG
    template<typename T>
    using ObjectReference = detail::ObjectReferenceDebug<T>;
#else
    template<typename T>
    using ObjectReference = detail::ObjectReferenceRelease<T>;
#endif /* ISL_DEBUG */
}// namespace isl

#endif /* ISL_PROJECT_OBJECT_REFERENCE_HPP */
