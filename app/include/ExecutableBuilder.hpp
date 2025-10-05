#pragma once

#include <string>

class ExecutableBuilder {
  public:
    bool Assemble(const std::string &asm_file, const std::string &obj_file) const;
    bool Link(const std::string &obj_file, const std::string &exe_file) const;
};
