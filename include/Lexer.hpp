#ifndef LEXER_HPP
#define LEXER_HPP

#include "Token.hpp"
#include <functional>

namespace Lexer
{

  using TokenHandler = std::function<void(const Token::TokenData&)>;

  inline auto constexpr FindKeywordOrIdentifier(const std::string_view Toks)
  -> Token::TokenType {


    if
    (
      Toks == "type"
    ) return Token::TokenType::Type;

    if
    (
      Toks == "enum"
    ) return Token::TokenType::Enum;

    return Token::TokenType::Identifier;

  }

  void Lex(const std::string_view k_Cbuffer, const TokenHandler& handleToken);
}

#endif
