#include "Driver.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

void Driver::LogError(Error const &err) const {
    std::cerr << name_ << ": error: " << err.message;
    if (err.ec) std::cerr << " (" << err.ec.message() << ')';
    std::cerr << '\n';
}

int Driver::Run(int argc, char *argv[]) {
    if (argc > 0 && argv[0]) {
        std::filesystem::path p{argv[0]};
        name_ = p.filename().string();
        if (name_.empty()) name_ = "boomerang";
    }

    auto maybe_path = parser_.Parse(argc, argv);
    if (!maybe_path) {
        LogError(maybe_path.error());
        return EXIT_FAILURE;
    }

    std::filesystem::path input_file = *maybe_path;
    auto contents = reader_.Read(input_file);
    if (!contents) {
        LogError(contents.error());
        return EXIT_FAILURE;
    }

    auto tokens = tokenizer_.Tokenize(*contents);
    std::ofstream out("out.asm", std::ios::out | std::ios::trunc);
    out << asm_gen_.Generate(tokens);
    out.close();

    if (!builder_.Assemble("out.asm", "out.o") || !builder_.Link("out.o", "out"))
        return EXIT_FAILURE;

    return EXIT_SUCCESS;
}
