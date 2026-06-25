#include "Parser.hpp"
#include <print>

namespace Enum {

  Parser::Parser
  (
    std::string_view SourceView,
    std::string_view FileName
  ) noexcept : Sv_SourceView_(SourceView), Sv_FileName_(FileName) {

    this->Toks_.reserve(512);

  }

  auto Parser::ReportError
  (
    const Token::TokenData &Tok,
    const std::string_view Msg
  ) const noexcept -> void
  {

    std::println
    (
      stderr,
      "\033[1m{}:{}:{}:\033[0m \033[1;31m error:\033[0m {}\033[0m",
      this->Sv_FileName_,
      Tok.Lexer_Size_t_Line_,
      Tok.Lexer_Size_t_Column_,
      Msg
    );

    if
    (
      Tok.Lexer_Size_t_Line_ == 0 ||
      this->Sv_SourceView_.empty()
    ) [[
      unlikely
    ]] std::exit(1);

    size_t st_LineStart_ = Tok.Lexer_Size_t_Offset_;

    while
    (
      st_LineStart_ > 0 &&
      this->Sv_SourceView_[st_LineStart_ - 1] != '\n'
    ) st_LineStart_--;

    size_t st_LineEnd_ = Tok.Lexer_Size_t_Offset_;

    while
    (
      st_LineEnd_ < this->Sv_SourceView_.size() &&
      this->Sv_SourceView_[st_LineEnd_] != '\n'
    ) st_LineEnd_++;

    std::string_view Sv_SourceLine_ = this->Sv_SourceView_.subview
    (
      st_LineStart_,
      st_LineEnd_ - st_LineStart_
    );

    std::println
    (
      stderr,
      "{:<5} | {}",
      Tok.Lexer_Size_t_Line_,
      Sv_SourceLine_
    );

    const size_t kSt_PadLen_ = (Tok.Lexer_Size_t_Column_ > 0)
                                ? (Tok.Lexer_Size_t_Column_ - 1)
                                : 0u;

    const std::string Pad(kSt_PadLen_, ' ');
    std::string Underline = "^";

    if
    (
      Tok.Sv_Val_.length() > 1
    ) Underline.append(Tok.Sv_Val_.length() - 1, '~');

    std::println
    (
      stderr,
      "      | {}\033[1;31m{}\033[0m",
      Pad,
      Underline
    );

    std::exit(1);

  }

  auto Parser::Advance() noexcept -> Token::TokenData
  {

    if
    (
      !IsAtEnd()
    ) Cursor_++;

    return this->Toks_[this->Cursor_ - 1];

  }

  auto Parser::Peek() noexcept -> Token::TokenData
  {

    if
    (
      this->IsAtEnd()
    ) {

      static constexpr Token::TokenData EofToken
      {
        .Type = Token::TokenType::EndOfFile,
        .Sv_Val_ = "",
        .Lexer_Size_t_Line_ = 0,
        .Lexer_Size_t_Column_ = 0,
        .Lexer_Size_t_Offset_ = 0
      };

      return EofToken;

    }

    return this->Toks_[this->Cursor_];

  }


  auto Parser::Consume
  (
    const Token::TokenType Toks,
    std::string_view Msg
  ) -> Token::TokenData
  {

    if
    (
      this->Peek().Type == Toks
    ) return Advance();

    this->ReportError(Peek(), Msg);

  }


  auto Parser::EnumClass() noexcept
  -> std::optional<EnumASTNode>
  {

    std::vector<std::string_view> EnumTokens;
    EnumTokens.reserve(16);

    if
    (
      this->Peek().Type == Token::TokenType::Type
    ) [[
      likely
    ]] this->Consume
    (
      Token::TokenType::Type,
      std::format
      (
        "Unexpected Error, couldn't consume 'type "
        "deliberately got {} which is crucial and unknown to the Transpiler.",
        Peek().Sv_Val_
      )
    );

    auto EnumTok = this->Consume
    (
      Token::TokenType::Identifier,
      "Malformed type field found in the line, couldn't identify the code."
    );

    std::string_view type = "uint32";
    if
    (
      this->Peek().Type == Token::TokenType::Enum
    ) [[
      likely
    ]] {

      this->Advance();

      if
      (
        this->Peek().Type == Token::TokenType::LeftParen
      ) [[
        unlikely
      ]] {

        this->Consume
        (
          Token::TokenType::LeftParen,
          std::format
          (
            "Malformed opening paren found in the line, deliberately got "
            "{} which is crucial and invalid to the Transpiler.", Peek().Sv_Val_
          )
        );

        if
        (
          Peek().Sv_Val_ == "const"
        ) [[
          unlikely
        ]] this->ReportError

        (
          Peek(),
          "Malformed type with 'const', enum must be a fixed, non-cv-qualifed integral type."
        );

        if
        (
          this->Peek().Sv_Val_ == "*" ||
          this->Peek().Sv_Val_ == "&"
        ) [[
          unlikely
        ]] this->ReportError

        (
          this->Peek(), "Unsafe pointer or reference found inside enum - cannot do a conversion."
        );


        this->Consume
        (
          Token::TokenType::Identifier,
          std::format("invalid type initialization, expected a type identifier "
          "in the paren scope but got this instead: {}", Peek().Sv_Val_)
        );

        type = this->Toks_[this->Cursor_ - 1].Sv_Val_;


        this->Consume
        (
          Token::TokenType::RightParen,
          "expected right-paren after type initialization. Malformed enum-initialization"
        );

      }

      this->Consume
      (
        Token::TokenType::LeftBrace,
        "invalid body initialization, expected '{'"
      );

      while
      (
        !this->IsAtEnd() &&
        this->Peek().Type != Token::TokenType::RightBrace
      ) {

        if
        (
          this->Peek().Type == Token::TokenType::Comma
        ) this->ReportError(
          this->Peek(),
          "Unexpected comma! It looks like you have a stray comma or an empty variant."
        );

        auto TokenTok = this->Consume
        (
          Token::TokenType::Identifier,
          "Expected an identifier in the body of Enumeration."
        );

        if
        (
          this->Peek().Sv_Val_ == "::" ||
          this->Peek().Sv_Val_ == "." ||
          this->Peek().Type == Token::TokenType::LeftParen
        ) [[
          unlikely
        ]] this->ReportError

        (
          this->Peek(),
          std::format
          (
            "Unexpected '{}' after '{}' - enum variants must be plain identifier",
            Peek().Sv_Val_, TokenTok.Sv_Val_
          )
        );

        EnumTokens.emplace_back(TokenTok.Sv_Val_);

        if
        (
          this->Peek().Type != Token::TokenType::Comma
        ) break;
        this->Advance();

        if
        (
          !this->IsAtEnd() &&
          this->Peek().Type == Token::TokenType::RightBrace
        ) [[
          unlikely
        ]] this->ReportError

        (
          this->Toks_[this->Cursor_],
          std::format
          (
            "Trailing comma after '{}' is not allowed in enumeration body",
            EnumTokens.back()
          )
        );

      }

      Consume
      (
        Token::TokenType::RightBrace,
        "Expected '}' to end the enum body."
      );

      return EnumASTNode
      {
        .Name = EnumTok.Sv_Val_,
        .Type = type,
        .Body = std::move(EnumTokens)
      };

    }

    else [[
      /* nullAttr */
    ]] {
      this->Cursor_ -= 2;
      return std::nullopt;
    }

  }

  auto Parser::ParseProgram() noexcept -> std::vector<EnumASTNode>
  {
    std::vector<EnumASTNode> AST;
    AST.reserve(128);

    while
    (
      !this->IsAtEnd()
    ) {

      if
      (
        this->Peek().Type == Token::TokenType::Type
      ) {

        if
        (
          auto EnumOpt = EnumClass(); EnumOpt.has_value()
        ) {

          AST.emplace_back(
            std::move(EnumOpt.value())
          );
          continue;

        }

        this->Advance();
        continue;

      }

      this->Advance();
    }

    return AST;

  }
}
