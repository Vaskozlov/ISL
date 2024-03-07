export module isl.io;

export import isl.collection;
export import isl.safety;

export namespace isl::io
{
    enum class IOError
    {
        FILE_NOT_EXISTS,
        IS_NOT_A_FILE,
        UNABLE_TO_OPEN_FILE
    };

    auto readFile(const std::filesystem::path &path) -> std::expected<std::string, IOError>
    {
        if (!std::filesystem::exists(path)) {
            return std::unexpected(IOError::FILE_NOT_EXISTS);
        }

        if (std::filesystem::is_directory(path)) {
            return std::unexpected(IOError::IS_NOT_A_FILE);
        }

        auto stream = std::ifstream(path);

        if (!stream.is_open()) {
            return std::unexpected(IOError::UNABLE_TO_OPEN_FILE);
        }

        auto buffer = std::stringstream{};
        buffer << stream.rdbuf();

        return buffer.str();
    }

    template<StringLike<char>... Ts>
    auto
        writeToFile(const std::filesystem::path &path, Ts &&...args) -> std::expected<void, IOError>
    {
        if (std::filesystem::exists(path) && std::filesystem::is_directory(path)) {
            return std::unexpected(IOError::IS_NOT_A_FILE);
        }

        auto out = std::ofstream{path.string()};
        const auto file_tracker = isl::Raii([&out]() {
            out.close();
        });

        if (!out.is_open()) {
            return std::unexpected(IOError::UNABLE_TO_OPEN_FILE);
        }

        (out.write(std::data(args), isl::as<long>(std::size(args))), ...);
        return {};
    }
}// namespace isl::io
