#ifndef ISL_SMALL_FUNCTION_HPP
#define ISL_SMALL_FUNCTION_HPP

#include <isl/isl.hpp>

namespace isl
{
    template<typename T, std::size_t N = sizeof(std::size_t) * 4>
    class SmallFunction;

    template<typename Ret, typename... Args, std::size_t N>
    class SmallFunction<Ret(Args...), N>
    {
        struct InvokerBase
        {
            InvokerBase() = default;

            InvokerBase(const InvokerBase &) = delete;

            InvokerBase(InvokerBase &&) = delete;

            virtual ~InvokerBase() = default;

            auto operator=(const InvokerBase &) -> InvokerBase & = delete;

            auto operator=(InvokerBase &&) -> InvokerBase & = delete;

            ISL_DECL virtual auto invoke(Args &&...args) const -> Ret = 0;

            constexpr virtual auto moveConstruct(void *destination) -> void = 0;

            constexpr virtual auto copyConstruct(void *destination) const -> void = 0;
        };

        template<typename T>
        struct Invoker final : InvokerBase
        {
            mutable T function;

            explicit Invoker(T function)
              : function{std::move(function)}
            {}

            ISL_DECL auto invoke(Args &&...args) const -> Ret override
            {
                return function(std::forward<Args>(args)...);
            }

            constexpr auto moveConstruct(void *destination) -> void override
            {
                new (destination) Invoker{std::move(function)};
            }

            constexpr auto copyConstruct(void *destination) const -> void override
            {
                new (destination) Invoker{function};
            }
        };

        std::byte smallStorage[N];// NOLINT (modernize-avoid-c-arrays)

    public:
        template<typename F>
            requires std::invocable<F, Args...>
        // NOLINTNEXTLINE (cppcoreguidelines-pro-type-member-init)
        ISL_DECL explicit SmallFunction(F &&function)
            requires(sizeof(Invoker<F>) <= sizeof(smallStorage))
        {
            new (smallStorage) Invoker<F>{std::forward<F>(function)};
        }

        // NOLINTNEXTLINE (cppcoreguidelines-pro-type-member-init)
        ISL_DECL SmallFunction(const SmallFunction &other)
        {
            other.getInvokerBase()->copyConstruct(smallStorage);
        }

        // NOLINTNEXTLINE (cppcoreguidelines-pro-type-member-init)
        ISL_DECL SmallFunction(SmallFunction &&other) noexcept
        {
            other.getInvokerBase()->moveConstruct(smallStorage);
        }

        constexpr ~SmallFunction()
        {
            std::destroy_at(getInvokerBase());
        }

        constexpr auto operator=(const SmallFunction &other) -> SmallFunction &
        {
            if (this == &other) {
                return *this;
            }

            std::destroy_at(getInvokerBase());
            other.getInvokerBase()->copyConstruct(smallStorage);

            return *this;
        }

        constexpr auto operator=(SmallFunction &&other) noexcept -> SmallFunction &
        {
            if (this == &other) {
                return *this;
            }

            std::destroy_at(getInvokerBase());
            other.getInvokerBase()->moveConstruct(smallStorage);

            return *this;
        }

        template<typename... InvokeArgs>
        ISL_DECL auto operator()(InvokeArgs &&...args) const -> Ret
        {
            return getInvokerBase()->invoke(static_cast<Args>(args)...);
        }

    private:
        ISL_DECL auto getInvokerBase() noexcept -> InvokerBase *
        {
            // NOLINTNEXTLINE (cppcoreguidelines-pro-type-reinterpret-cast)
            return reinterpret_cast<InvokerBase *>(&smallStorage[0]);
        }

        ISL_DECL auto getInvokerBase() const noexcept -> const InvokerBase *
        {
            // NOLINTNEXTLINE (cppcoreguidelines-pro-type-reinterpret-cast)
            return reinterpret_cast<const InvokerBase *>(&smallStorage[0]);
        }
    };
}// namespace isl

#endif /* ISL_SMALL_FUNCTION_HPP */
