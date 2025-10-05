#pragma once

#include <vector>
#include <string>

#include "Token.hpp"

class AsmGenerator {
  public:
    std::string Generate(const std::vector<Token> &tokens) const;
};
