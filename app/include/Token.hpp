#pragma once

#include <optional>

enum class TokenType { RETURN_TK, NUMBER_TK, SEMICOLON_TK };

struct Token {
    TokenType type;
    std::optional<int> value;
};