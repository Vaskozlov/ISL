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
    auto read(const std::filesystem::path &filename) -> std::string;

    template<StringLike<char>... Ts>
    auto write(const std::filesystem::path &filename, Ts &&...args) -> void
    {
        auto out = std::ofstream{filename};

        const auto file_closer = Raii([&out]() {
            out.close();
        });

        (out.write(std::data(args), as<long>(std::size(args))), ...);
    }
}// namespace isl::io

#endif /* ISL_PROJECT_IO_HPP */
