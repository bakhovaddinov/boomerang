#include "Tokenizer.hpp"

#include <cctype>
#include <charconv>
#include <optional>

std::vector<Token> Tokenizer::Tokenize(std::string_view source) const {
    std::vector<Token> tokens;
    tokens.reserve(std::min<size_t>(64, source.size() / 2 + 1));

    size_t i = 0;
    const size_t n = source.size();

    while (i < n) {
        unsigned char ch = static_cast<unsigned char>(source[i]);

        if (std::isspace(ch)) {
            ++i;
            continue;
        }

        if (std::isalpha(ch) || ch == '_') {
            size_t j = i + 1;
            while (j < n &&
                   (std::isalnum(static_cast<unsigned char>(source[j])) || source[j] == '_'))
                ++j;
            auto tok = source.substr(i, j - i);
            if (tok == "return") tokens.push_back({TokenType::RETURN_TK, std::nullopt});
            i = j;
            continue;
        }

        if (std::isdigit(ch)) {
            size_t j = i + 1;
            while (j < n && std::isdigit(static_cast<unsigned char>(source[j])))
                ++j;
            Token t{TokenType::NUMBER_TK, std::nullopt};
            int val = 0;
            auto [ptr, ec] = std::from_chars(source.data() + i, source.data() + j, val);
            if (ec == std::errc()) t.value = val;
            tokens.push_back(std::move(t));
            i = j;
            continue;
        }

        if (ch == ';') {
            tokens.push_back({TokenType::SEMICOLON_TK, std::nullopt});
            ++i;
            continue;
        }
        ++i;
    }

    return tokens;
}
