#ifndef ISL_SHARED_LIB_LOADER_HPP
#define ISL_SHARED_LIB_LOADER_HPP

#include <expected>
#include <filesystem>
#include <fmt/core.h>

#if __has_include(<dlfcn.h>)
#    include <dlfcn.h>

namespace isl
{
    class SharedLibLoader
    {
    private:
        std::filesystem::path path;
        void *handler{};

        explicit SharedLibLoader(std::filesystem::path library_path, void *library_handler)
          : path{std::move(library_path)}
          , handler{library_handler}
        {}

    public:
        SharedLibLoader(const SharedLibLoader &) = delete;

        SharedLibLoader(SharedLibLoader &&other) noexcept
          : path{std::move(other.path)}
          , handler{std::exchange(other.handler, nullptr)}
        {}

        auto operator=(SharedLibLoader &&other) noexcept -> SharedLibLoader &
        {
            std::swap(path, other.path);
            std::swap(handler, other.handler);
            return *this;
        }

        auto operator=(const SharedLibLoader &) -> SharedLibLoader & = delete;

        ~SharedLibLoader()
        {
            close();
        }

        auto close() -> void
        {
            if (hasLibrary()) {
                dlclose(handler);
                handler = nullptr;
            }
        }

        static auto loadLibrary(const std::filesystem::path &path)
            -> std::expected<SharedLibLoader, std::string>
        {
            void *handle = dlopen(path.c_str(), RTLD_LOCAL | RTLD_NOW);

            if (handle == nullptr) {
                return std::unexpected(fmt::format("Failed to load shared library: {}", dlerror()));
            }

            return SharedLibLoader(path, handle);
        }

        auto getSymbol(const std::string &symbol) const -> std::expected<void *, std::string>
        {
            auto *pointer = dlsym(handler, symbol.c_str());

            if (pointer == nullptr) {
                return std::unexpected(fmt::format("Failed to load shared library: {}", dlerror()));
            }

            return pointer;
        }

        auto reload() -> std::expected<void, std::string>
        {
            auto *new_handler = dlopen(path.c_str(), RTLD_LOCAL | RTLD_NOW);

            if (new_handler == nullptr) {
                return std::unexpected(fmt::format("Failed to load shared library: {}", dlerror()));
            }

            close();
            handler = new_handler;

            return {};
        }

        auto hasLibrary() const -> bool
        {
            return handler != nullptr;
        }
    };
} // namespace isl

#endif

#endif /* ISL_SHARED_LIB_LOADER_HPP */
