#include "FileHandler.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"
#include <ranges>
// #include <print>

auto main
(
  int argc,
  char *argv[]
) -> int32_t {

  std::span<char const *const> _arguments
  {
    std::views::counted
    (
      argv, argc
    )
  };

  if
  (
    _arguments.size() < 2
  ) {
    // std::println("Error: A file input pathway is required!");
    exit(1);
  }

  FileHandler::FileOpener file{_arguments[1]};

  Enum::Parser Parse
  {
    file.view(),
    _arguments[1]
  };

  Lexer::Lex
  (
    file.view(),
    [&Parse]
    (
      const Token::TokenData& token
    ) noexcept
    {
      // std::println("{:<5} | {:<5} | {:<5} | {:<5}",
      // token.Lexer_Size_t_Line_, token.Lexer_Size_t_Column_, token.Lexer_Size_t_Offset_, token.Sv_Val_);
      Parse.Append(token);
    }
  );

  Parse.ParseProgram();
}
