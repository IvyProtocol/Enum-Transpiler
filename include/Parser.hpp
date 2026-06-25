#ifndef PARSER_HPP
#define PARSER_HPP

#include "Token.hpp"
#include <optional>
#include <string_view>
#include <vector>

namespace Enum
{

  class EnumASTNode
  {

    public:
      std::string_view Name, Type;
      std::vector<std::string_view> Body;
  };


  class Parser
  {
    private:

      std::string_view Sv_SourceView_ {}, Sv_FileName_ {};
      size_t Cursor_ {};
      std::vector<Token::TokenData> Toks_;

      [[nodiscard]] auto Peek() const noexcept -> Token::TokenType;
      auto Consume
      (
        const Token::TokenType Toks,
        std::string_view Msg
      ) -> Token::TokenData;

      auto IsAtEnd() const noexcept -> bool
      {

        return this->Cursor_ >= this->Toks_.size() ||
                this->Toks_[this->Cursor_].Type == Token::TokenType::EndOfFile;

      }

      auto Advance() noexcept -> Token::TokenData;
      auto Peek() noexcept -> Token::TokenData;
      [[noreturn]] auto ReportError
      (
        const Token::TokenData &Tok,
        const std::string_view Msg
      ) const noexcept -> void;

    public:

      explicit Parser
      (
        std::string_view SourceView,
        std::string_view FileName
      ) noexcept;

      auto EnumClass() noexcept
      -> std::optional<EnumASTNode>;

      auto ParseProgram() noexcept -> std::vector<EnumASTNode>;
      auto Append(const Token::TokenData Tok) noexcept -> void
      {
        this->Toks_.emplace_back(std::move(Tok));
      }

  };
}

#endif
