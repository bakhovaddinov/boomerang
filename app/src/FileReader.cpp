#include "FileReader.hpp"

#include <fstream>
#include <filesystem>

std::expected<std::string, Error> FileReader::Read(std::filesystem::path const &p) const {
    std::error_code ec;
    if (!std::filesystem::exists(p, ec))
        return std::unexpected(Error{ErrKind::NotExist, "file does not exist", ec});
    if (!std::filesystem::is_regular_file(p, ec))
        return std::unexpected(Error{ErrKind::NotRegular, "not a regular file", ec});

    std::ifstream in(p, std::ios::binary | std::ios::ate);
    if (!in)
        return std::unexpected(Error{ErrKind::OpenFailed, "failed to open file",
                                     std::make_error_code(std::errc::io_error)});

    auto size = in.tellg();
    if (size < 0)
        return std::unexpected(Error{ErrKind::Other, "could not determine file size", {}});

    std::string data(static_cast<size_t>(size), '\0');
    in.seekg(0);
    in.read(data.data(), static_cast<std::streamsize>(data.size()));
    if (!in)
        return std::unexpected(Error{ErrKind::ReadFailed, "read failed or incomplete",
                                     std::make_error_code(std::errc::io_error)});
    return data;
}
