#include "ExecutableBuilder.hpp"

#include <cstdlib>
#include <iostream>
#include <filesystem>

bool ExecutableBuilder::Assemble(const std::string &asm_file, const std::string &obj_file) const {
    std::clog << "[Assemble] asm_file: " << asm_file << "\n";
    std::clog << "[Assemble] obj_file: " << obj_file << "\n";

    if (!std::filesystem::exists(asm_file)) {
        std::cerr << "[Assemble] ERROR: assembly file does not exist: " << asm_file << "\n";
        return false;
    }

    int rc = std::system(("nasm -f macho64 -o " + obj_file + " " + asm_file).c_str());
    if (rc != 0) {
        int code = (rc == -1) ? -1 : (rc >> 8);
        std::cerr << "[Assemble] nasm failed (rc=" << code << ")\n";
        return false;
    }

    if (!std::filesystem::exists(obj_file)) {
        std::cerr << "[Assemble] ERROR: object file was not created: " << obj_file << "\n";
        return false;
    }

    std::clog << "[Assemble] Success: " << obj_file << " created\n";
    return true;
}

bool ExecutableBuilder::Link(const std::string &obj_file, const std::string &exe_file) const {
    std::clog << "[Link] obj_file: " << obj_file << "\n";
    std::clog << "[Link] exe_file: " << exe_file << "\n";

    if (!std::filesystem::exists(obj_file)) {
        std::cerr << "[Link] ERROR: object file does not exist: " << obj_file << "\n";
        return false;
    }

    int rc = std::system(("clang -arch x86_64 " + obj_file + " -o " + exe_file).c_str());
    if (rc != 0) {
        int code = (rc == -1) ? -1 : (rc >> 8);
        std::cerr << "[Link] link failed (rc=" << code << ")\n";
        return false;
    }

    if (!std::filesystem::exists(exe_file)) {
        std::cerr << "[Link] ERROR: executable was not created: " << exe_file << "\n";
        return false;
    }

    std::clog << "[Link] Success: " << exe_file << " created\n";
    return true;
}
