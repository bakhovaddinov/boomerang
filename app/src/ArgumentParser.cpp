#include "ArgumentParser.hpp"

std::expected<std::filesystem::path, Error> ArgumentParser::Parse(int argc, char *argv[]) const {
    if (argc < 2) return std::unexpected(Error{ErrKind::MissingInput, "missing input file", {}});
    std::filesystem::path p{argv[1]};
    if (p.empty()) return std::unexpected(Error{ErrKind::EmptyPath, "empty input path", {}});
    if (p.extension() != ".bmg")
        return std::unexpected(
            Error{ErrKind::BadExtension, "input must have a .bmg extension", {}});
    return p;
}
