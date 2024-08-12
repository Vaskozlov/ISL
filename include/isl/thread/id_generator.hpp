#ifndef ISL_PROJECT_ID_GENERATOR_HPP
#define ISL_PROJECT_ID_GENERATOR_HPP

#include <atomic>
#include <isl/isl.hpp>

namespace isl::thread
{
    template<std::unsigned_integral T = Id>
    class IdGenerator
    {
    private:
        std::atomic<T> id;
        T initialValue;

    public:
        ISL_DECL explicit IdGenerator(T start)
          : id{start}
          , initialValue{start}
        {}

        [[nodiscard]] auto next() -> T
        {
            return id.fetch_add(1, std::memory_order_relaxed);
        }

        [[nodiscard]] auto current() const -> T
        {
            return id.load(std::memory_order_relaxed);
        }

        auto restore() -> void
        {
            id.store(initialValue, std::memory_order_relaxed);
        }
    };
}// namespace isl::thread

#endif /* ISL_PROJECT_ID_GENERATOR_HPP */
