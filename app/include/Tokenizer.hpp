#pragma once

#include <vector>
#include "Token.hpp"

class Tokenizer {
  public:
    std::vector<Token> Tokenize(std::string_view source) const;
};
