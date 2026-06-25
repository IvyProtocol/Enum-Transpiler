#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <cstdint>
#include <string_view>

namespace Token
{

  enum class [[
    /* nullAttr */
  ]] TokenType : std::int32_t {

    /*
    * Keywords
    */

    Type,        // TypeKeyword
    Enum,        // EnumKeyword


    /*
    * Separator/Temrinator
    */

    Comma,       // CommaSeparator

    /*
    * Brackets
    */

    LeftParen,  // LeftParen
    RightParen, // RightParen

    LeftBrace,  // Leftbrace
    RightBrace, // RightBrace


    /*
    * Literals
    */

    IntLiteral,       // IntLiteral
    FloatLiteral,     // FloatLiteral
    StringLiteral,    // StringLiteral

    /*
    * Default
    */

    Identifier, // IdentifierMarker
    Unknown,    // UnknownMarker
    EndOfFile,  // EndOfFileMarker

  };

  class [[
    /* nullAttr*/
  ]] TokenData {

    public:
      TokenType Type;
      long : (8 * 4);
      std::string_view Sv_Val_;
      std::size_t Lexer_Size_t_Line_;
      std::size_t Lexer_Size_t_Column_;
      std::size_t Lexer_Size_t_Offset_;

  };
} // namespace Token

#endif
