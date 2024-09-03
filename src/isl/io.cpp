#include <fmt/std.h>
#include <isl/io.hpp>

namespace isl::io
{
    auto read(const std::filesystem::path &filename) -> std::string
    {
        auto stream = std::ifstream{filename};

        if (!stream.is_open()) {
            throw std::invalid_argument{fmt::format("Failed to open file {}", filename)};
        }

        auto buffer = std::stringstream{};
        buffer << stream.rdbuf();

        return buffer.str();
    }
}// namespace isl::io
