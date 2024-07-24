#ifndef ISL_PROJECT_IO_HPP
#define ISL_PROJECT_IO_HPP

#include <filesystem>
#include <fstream>
#include <isl/isl.hpp>
#include <isl/raii.hpp>
#include <isl/string_view.hpp>
#include <istream>
#include <sstream>

namespace isl::io
{
    inline auto readFile(const std::filesystem::path &path) -> std::string
    {
        auto stream = std::ifstream(path);

        if (!stream.is_open()) {
            throw std::invalid_argument("Failed to open file");
        }

        auto buffer = std::stringstream{};
        buffer << stream.rdbuf();

        return buffer.str();
    }

    template<StringLike<char>... Ts>
    auto writeToFile(const std::string &filename, Ts &&...args) -> void
    {
        auto out = std::ofstream{filename};

        const auto file_closer = Raii([&out]() {
            out.close();
        });

        (out.write(std::data(args), as<long>(std::size(args))), ...);
    }
}// namespace isl::io

#endif /* ISL_PROJECT_IO_HPP */
