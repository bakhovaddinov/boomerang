#pragma once

#include "Tokenizer.hpp"
#include "ArgumentParser.hpp"
#include "FileReader.hpp"
#include "AsmGenerator.hpp"
#include "ExecutableBuilder.hpp"
#include "Error.hpp"

#include <string>

class Driver {
    std::string name_;
    ArgumentParser parser_;
    FileReader reader_;
    Tokenizer tokenizer_;
    AsmGenerator asm_gen_;
    ExecutableBuilder builder_;

  public:
    int Run(int argc, char *argv[]);

  private:
    void LogError(Error const &err) const;
};
