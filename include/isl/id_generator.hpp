#ifndef ISL_PROJECT_ID_GENERATOR_HPP
#define ISL_PROJECT_ID_GENERATOR_HPP

#include <atomic>
#include <isl/isl.hpp>

namespace isl
{
    template<std::unsigned_integral T = Id>
    class IdGenerator
    {
    private:
        std::atomic<T> id;

    public:
        ISL_DECL explicit IdGenerator(T start)
          : id{start}
        {}

        [[nodiscard]] auto next() -> T
        {
            return id.fetch_add(1, std::memory_order_relaxed);
        }

        [[nodiscard]] auto current() const -> T
        {
            return id.load(std::memory_order_relaxed);
        }
    };
}// namespace isl

#endif /* ISL_PROJECT_ID_GENERATOR_HPP */
