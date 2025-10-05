#pragma once

#include <expected>
#include <filesystem>
#include <string>

#include "Error.hpp"

class FileReader {
  public:
    std::expected<std::string, Error> Read(std::filesystem::path const &p) const;
};