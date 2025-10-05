#pragma once

#include <expected>
#include <filesystem>

#include "Error.hpp"

class ArgumentParser {
  public:
    std::expected<std::filesystem::path, Error> Parse(int argc, char *argv[]) const;
};
