#include <algorithm>
#include <cctype>
#include <charconv>
#include <cstdlib>
#include <expected>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <syncstream>
#include <system_error>
#include <vector>

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

enum class TokenType { RETURN_TK, NUMBER_TK, SEMICOLON_TK };

struct Token {
  TokenType type;
  std::optional<int> value;
};

struct Error {
  ErrKind kind;
  std::string message;
  std::error_code ec{};
};

static void log_error(std::string_view program, Error const& err) {
  std::cerr << std::format("{}: error: {}", program, err.message);
  if (err.ec) std::cerr << " (" << err.ec.message() << ')';
  std::cerr << '\n';
  std::cerr << std::format("usage: {} <file.bmg>\n", program);
}

static std::expected<std::string, Error> parse_args(int argc, char* argv[]) {
  if (argc < 2) {
    return std::unexpected<Error>{
        Error{ErrKind::MissingInput, "missing input file", {}}};
  }

  std::string_view pathv{argv[1]};
  if (pathv.empty()) {
    return std::unexpected<Error>{
        Error{ErrKind::EmptyPath, "empty input path", {}}};
  }

  std::filesystem::path p{pathv};
  if (p.extension() != ".bmg") {
    return std::unexpected<Error>{
        Error{ErrKind::BadExtension, "input must have a .bmg extension", {}}};
  }

  return std::string{p.string()};
}

static std::expected<std::string, Error> read_file(
    std::filesystem::path const& p) {
  std::error_code ec;
  if (!std::filesystem::exists(p, ec)) {
    return std::unexpected<Error>{
        Error{ErrKind::NotExist, "file does not exist", ec}};
  }
  if (!std::filesystem::is_regular_file(p, ec)) {
    return std::unexpected<Error>{
        Error{ErrKind::NotRegular, "not a regular file", ec}};
  }

  std::ifstream in(p, std::ios::binary | std::ios::ate);
  if (!in) {
    return std::unexpected<Error>{
        Error{ErrKind::OpenFailed, "failed to open file",
              std::make_error_code(std::errc::io_error)}};
  }

  auto size = in.tellg();
  if (size < 0) {
    return std::unexpected<Error>{
        Error{ErrKind::Other, "could not determine file size", {}}};
  }

  std::string data;
  data.resize(static_cast<std::size_t>(size));
  in.seekg(0);
  in.read(data.data(), static_cast<std::streamsize>(data.size()));
  if (!in) {
    return std::unexpected<Error>{
        Error{ErrKind::ReadFailed, "read failed or incomplete",
              std::make_error_code(std::errc::io_error)}};
  }
  return data;
}

static int map_exit_code(Error const&) { return EXIT_FAILURE; }

std::vector<Token> tokenize(std::string_view source) {
  std::vector<Token> tokens;
  tokens.reserve(std::min<size_t>(64, source.size() / 2 + 1));

  try {
    size_t i = 0;
    const size_t n = source.size();

    while (i < n) {
      const unsigned char ch = static_cast<unsigned char>(source[i]);

      if (std::isspace(ch)) {
        ++i;
        continue;
      }

      if (ch == '/' && i + 1 < n) {
        if (source[i + 1] == '/') {
          i += 2;
          while (i < n && source[i] != '\n') ++i;
          continue;
        } else if (source[i + 1] == '*') {
          i += 2;
          while (i + 1 < n && !(source[i] == '*' && source[i + 1] == '/')) ++i;
          if (i + 1 < n) i += 2;
          continue;
        }
      }

      if (std::isalpha(ch) || ch == '_') {
        size_t j = i + 1;
        while (j < n) {
          unsigned char c2 = static_cast<unsigned char>(source[j]);
          if (std::isalnum(c2) || c2 == '_')
            ++j;
          else
            break;
        }
        auto tok = source.substr(i, j - i);
        if (tok == "return") tokens.push_back({TokenType::RETURN_TK});
        i = j;
        continue;
      }

      if (std::isdigit(ch)) {
        size_t j = i + 1;
        while (j < n && std::isdigit(static_cast<unsigned char>(source[j])))
          ++j;
        // parse integer without allocations/exceptions
        Token t;
        t.type = TokenType::NUMBER_TK;
        auto sv = source.substr(i, j - i);
        int val = 0;
        auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val);
        if (ec == std::errc()) t.value = val;
        tokens.push_back(std::move(t));
        i = j;
        continue;
      }

      if (ch == ';') {
        tokens.push_back({TokenType::SEMICOLON_TK});
        ++i;
        continue;
      }

      ++i;
    }
  } catch (const std::bad_alloc&) {
    return tokens;
  } catch (const std::exception&) {
    return tokens;
  }

  return tokens;
}

std::string tokens_to_asm(const std::vector<Token>& tokens) {
  std::stringstream output;

  output << "global _main\n";
  output << "extern _exit\n";
  output << "section __TEXT,__text\n";
  output << "_main:\n";

  for (size_t i = 0; i < tokens.size(); ++i) {
    const Token& token = tokens.at(i);
    if (token.type == TokenType::RETURN_TK) {
      if (i + 1 < tokens.size() &&
          tokens.at(i + 1).type == TokenType::NUMBER_TK &&
          tokens.at(i + 1).value.has_value()) {
        if (i + 2 < tokens.size() &&
            tokens.at(i + 2).type == TokenType::SEMICOLON_TK) {
          output << "    mov    rdi, " << tokens.at(i + 1).value.value()
                 << "\n";
          output << "    call   _exit\n";
        }
      }
    }
  }

  return output.str();
}

int main(int argc, char* argv[]) {
  std::string program = "boomerang";
  if (argc > 0 && argv[0]) {
    std::filesystem::path p{argv[0]};
    program = p.filename().string();
    if (program.empty()) program = "boomerang";
  }

  auto maybe_path = parse_args(argc, argv);
  if (!maybe_path) {
    Error err = maybe_path.error();
    log_error(program, err);
    return map_exit_code(err);
  }

  const std::string input_file = *maybe_path;
  std::clog << program << ": compiling '" << input_file << "'\n";

  auto contents = read_file(std::filesystem::path(input_file));
  if (!contents) {
    Error err = contents.error();
    log_error(program, err);
    return map_exit_code(err);
  }

  const std::string& file_data = *contents;
  std::clog << program << ": read " << file_data.size() << " bytes\n";

  auto tokens = tokenize(file_data);
  std::clog << program << ": tokenized to " << tokens.size() << " tokens\n";

  {
    std::ofstream file("out.asm", std::ios::out | std::ios::trunc);
    file << tokens_to_asm(tokens);
  }

  int rc = std::system("nasm -f macho64 -o out.o out.asm");
  if (rc != 0) {
    int code = (rc == -1) ? -1 : (rc >> 8);
    std::cerr << "nasm failed (rc=" << code << ")\n";
    return EXIT_FAILURE;
  }

  rc = std::system("clang -arch x86_64 out.o -o out");
  if (rc != 0) {
    int code = (rc == -1) ? -1 : (rc >> 8);
    std::cerr << "link failed (rc=" << code << ")\n";
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}