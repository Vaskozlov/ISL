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
        ~SharedLibLoader()
        {
            close();
        }

        void close()
        {
            if (handler != nullptr) {
                dlclose(handler);
                handler = nullptr;
            }
        }

        static std::expected<SharedLibLoader, std::string>
            load_library(const std::filesystem::path &path)
        {
            void *handle = dlopen(path.c_str(), RTLD_LOCAL | RTLD_NOW);

            if (handle == nullptr) {
                return std::unexpected(fmt::format("Failed to load shared library: {}", dlerror()));
            }

            return SharedLibLoader(path, handle);
        }

        std::expected<void *, std::string> getSymbol(const std::string &symbol) const
        {
            auto *pointer = dlsym(handler, symbol.c_str());

            if (pointer == nullptr) {
                return std::unexpected(fmt::format("Failed to load shared library: {}", dlerror()));
            }

            return pointer;
        }

        std::expected<void, std::string> reload()
        {
            auto *new_handler = dlopen(path.c_str(), RTLD_LOCAL | RTLD_NOW);

            if (new_handler == nullptr) {
                return std::unexpected(fmt::format("Failed to load shared library: {}", dlerror()));
            }

            close();
            handler = new_handler;

            return {};
        }
    };
} // namespace isl

#endif

#endif /* ISL_SHARED_LIB_LOADER_HPP */
