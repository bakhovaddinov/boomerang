#pragma once

#include <string>
#include <system_error>

enum class ErrKind {
  MissingInput,
  EmptyPath,
  BadExtension,
  NotExist,
  NotRegular,
  OpenFailed,
  ReadFailed,
  Other
};

struct Error {
  ErrKind kind;
  std::string message;
  std::error_code ec{};
};