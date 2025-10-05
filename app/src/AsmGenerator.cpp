#include "AsmGenerator.hpp"

#include <sstream>

std::string AsmGenerator::Generate(const std::vector<Token> &tokens) const {
    std::stringstream output;
    output << "global _main\nextern _exit\nsection __TEXT,__text\n_main:\n";

    for (size_t i = 0; i < tokens.size(); ++i) {
        const Token &token = tokens[i];
        if (token.type == TokenType::RETURN_TK) {
            if (i + 1 < tokens.size() && tokens[i + 1].type == TokenType::NUMBER_TK &&
                tokens[i + 1].value.has_value()) {
                if (i + 2 < tokens.size() && tokens[i + 2].type == TokenType::SEMICOLON_TK) {
                    output << "    mov    rdi, " << tokens[i + 1].value.value() << "\n";
                    output << "    call   _exit\n";
                }
            }
        }
    }

    return output.str();
}
