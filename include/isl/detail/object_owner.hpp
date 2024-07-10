#ifndef ISL_PROJECT_REFERENCE_HPP
#define ISL_PROJECT_REFERENCE_HPP

#include <isl/detail/concepts.hpp>
#include <isl/detail/object_reference.hpp>

namespace isl::detail
{
    template<typename T>
    class ObjectOwnerDebug : public T
    {
    private:
        std::shared_ptr<bool> isObjectDestroyed = std::make_shared<bool>(false);

    public:
        using T::T;

        ObjectOwnerDebug(const ObjectOwnerDebug &other)
          : T{other}
          , isObjectDestroyed{std::make_shared<bool>(*other.isObjectDestroyed)}
        {}

        ObjectOwnerDebug(ObjectOwnerDebug &&other) noexcept(std::is_nothrow_move_constructible_v<T>)
          : T{std::move(other)}
          , isObjectDestroyed{std::move(other.isObjectDestroyed)}
        {}

        ~ObjectOwnerDebug()
        {
            if (isObjectDestroyed != nullptr) {
                *isObjectDestroyed = true;
            }
        }

        auto operator=(const ObjectOwnerDebug &other) -> ObjectOwnerDebug &
        {
            if (this == &other) {
                return *this;
            }

            isl::as<T &>(*this) = other;
            *isObjectDestroyed = *other.isObjectDestroyed;

            return *this;
        }

        auto operator=(ObjectOwnerDebug &&other) noexcept(std::is_nothrow_move_assignable_v<T>)
            -> ObjectOwnerDebug &
        {
            isl::as<T &>(*this) = std::move(other);
            isObjectDestroyed = std::move(other.isObjectDestroyed);

            return *this;
        }

        [[nodiscard]] auto createReference() -> ObjectReferenceDebug<T>
        {
            return {*this, isObjectDestroyed};
        }

        [[nodiscard]] auto createReference() const -> ObjectReferenceDebug<const T>
        {
            return {*this, isObjectDestroyed};
        }
    };

    template<typename T>
    class WeakObjectOwnerDebug
    {
    private:
        T *object{};
        std::shared_ptr<bool> isObjectDestroyed = std::make_shared<bool>(false);

    public:
        explicit WeakObjectOwnerDebug(T &initial_object)
          : object{&initial_object}
        {}

        WeakObjectOwnerDebug(const WeakObjectOwnerDebug &other) = delete;

        WeakObjectOwnerDebug(WeakObjectOwnerDebug &&other) noexcept = default;

        ~WeakObjectOwnerDebug()
        {
            if (isObjectDestroyed != nullptr) {
                *isObjectDestroyed = true;
            }
        }

        auto operator=(const WeakObjectOwnerDebug &other) -> WeakObjectOwnerDebug & = delete;

        auto operator=(WeakObjectOwnerDebug &&other) noexcept -> WeakObjectOwnerDebug & = default;

        [[nodiscard]] auto createReference() -> ObjectReferenceDebug<T>
        {
            return {*object, isObjectDestroyed};
        }

        [[nodiscard]] auto createReference() const -> ObjectReferenceDebug<const T>
        {
            return {*this, isObjectDestroyed};
        }
    };

    template<typename T>
    class ObjectOwnerRelease : public T
    {
    public:
        using T::T;

        ISL_DECL auto createReference() noexcept -> ObjectReferenceRelease<T>
        {
            return ObjectReferenceRelease<T>{*this};
        }

        ISL_DECL auto createReference() const noexcept -> ObjectReferenceRelease<const T>
        {
            return ObjectReferenceRelease<T>{*this};
        }
    };

    template<typename T>
    class WeakObjectOwnerRelease
    {
    private:
        T *object;

    public:
        constexpr WeakObjectOwnerRelease(T &initial_object)
          : object{&initial_object}
        {}

        WeakObjectOwnerRelease(const WeakObjectOwnerRelease &other) = delete;

        WeakObjectOwnerRelease(WeakObjectOwnerRelease &&other) noexcept = default;

        ~WeakObjectOwnerRelease() = default;

        auto operator=(const WeakObjectOwnerRelease &other) -> WeakObjectOwnerRelease & = delete;

        auto operator=(WeakObjectOwnerRelease &&other) noexcept -> WeakObjectOwnerRelease & =
                                                                       default;

        ISL_DECL auto createReference() -> ObjectReferenceRelease<T>
        {
            return ObjectReferenceRelease<T>{*object};
        }

        ISL_DECL auto createReference() const -> ObjectReferenceRelease<const T>
        {
            return ObjectReferenceRelease<const T>{*object};
        }
    };
}// namespace isl::detail

namespace isl
{
#ifdef ISL_DEBUG
    template<typename T>
    using ObjectOwner = detail::ObjectOwnerDebug<T>;
#else
    template<typename T>
    using ObjectOwner = detail::ObjectOwnerRelease<T>;
#endif /* ISL_DEBUG */
}// namespace isl

#endif /* ISL_PROJECT_REFERENCE_HPP */
