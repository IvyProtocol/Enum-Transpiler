#include "Lexer.hpp"
#include "Token.hpp"
#include <cctype>

namespace Lexer {

  void Lex
  (
    std::string_view k_Cbuffer,
    const TokenHandler& handleToken
  ) {

    std::size_t FileOffset {}, FileLine {1}, FileColumn {1};

    auto ConsumeChar =[&]()
    {

      char c = k_Cbuffer[FileOffset];
      if
      (
        c == '\n'
      ) {

        FileLine++;
        FileColumn = 1;

      }

      else if
      (
        c == '\t'
      ) FileColumn += 4;

      else [[
        /* nullAttr */
      ]] FileColumn++;

      FileOffset++;
      return c;

    };

    while
    (
      FileOffset < k_Cbuffer.size()
    ) {

      char CurrentChar = k_Cbuffer[FileOffset];
      size_t TokenStartOffset = FileOffset;

      Token::TokenType Type = Token::TokenType::Unknown;

      if
      (
        std::isspace
        (
          static_cast<unsigned char>
          (
            CurrentChar
          )
        )
      ) ConsumeChar();

      else if
      (
        CurrentChar == '('
      ) {

        Type = Token::TokenType::LeftParen;
        ConsumeChar();

      }

      else if
      (
        CurrentChar == ')'
      ) {

        Type = Token::TokenType::RightParen;
        ConsumeChar();

      }

      else if
      (
        CurrentChar == '{'
      ) {

        Type = Token::TokenType::LeftBrace;
        ConsumeChar();

      }

      else if
      (
        CurrentChar == '}'
      ) {

        Type = Token::TokenType::RightBrace;
        ConsumeChar();

      }

      else if
      (
        CurrentChar == ','
      ) {

        Type = Token::TokenType::Comma;
        ConsumeChar();

      }

      else if
      (
        CurrentChar == '"' ||
        CurrentChar == '\''
      ) {

        auto QuoteType = CurrentChar;
        ConsumeChar();


        while
        (
          FileOffset < k_Cbuffer.size() &&
          k_Cbuffer[FileOffset] != QuoteType
        ) {

          if
          (
            k_Cbuffer[FileOffset] == '\\'
          ) ConsumeChar();

          ConsumeChar();

        }

        ConsumeChar();

        Type = Token::TokenType::StringLiteral;
      }

      else if
      (
        CurrentChar == '/'
      ) {

        if
        (
          FileOffset + 1 < k_Cbuffer.size() &&
          k_Cbuffer[FileOffset + 1] == '/'
        ) {

          ConsumeChar();
          ConsumeChar();

          while
          (
            FileOffset < k_Cbuffer.size() &&
            k_Cbuffer[FileOffset] != '\n'
          ) ConsumeChar();

        }

        else if
        (
          FileOffset + 1 < k_Cbuffer.size() &&
          k_Cbuffer[FileOffset + 1] == '*'
        ) {

          ConsumeChar();
          ConsumeChar();

          while
          (
            FileOffset + 1 < k_Cbuffer.size() &&
            (
              k_Cbuffer[FileOffset] != '*' &&
              k_Cbuffer[FileOffset + 1] != '/'
            )
          ) {

            ConsumeChar();

            if
            (
              k_Cbuffer[FileOffset] == '*' &&
              k_Cbuffer[FileOffset + 1] != '/'
            ) ConsumeChar();

          }


          if
          (
            FileOffset + 1 < k_Cbuffer.size()
          ) {

            ConsumeChar();
            ConsumeChar();

          }

          else [[
            /* nullAttr */
          ]] ConsumeChar();

        }

      }

      else if
      (
        CurrentChar == '`'
      ) {

        ConsumeChar();

        while
        (
          FileOffset < k_Cbuffer.size() &&
          k_Cbuffer[FileOffset] != '`'
        ) ConsumeChar();

        ConsumeChar();

      }

      else if
      (
        std::isdigit
        (
          static_cast<unsigned char>(CurrentChar)
        )
      ) {

        bool is_float {false};

        while
        (
          FileOffset < k_Cbuffer.size()
        ) {

          char Current = k_Cbuffer[FileOffset];
          if
          (
            std::isdigit
            (
              static_cast<unsigned char>(Current)
            )
          ) ConsumeChar();

          else if
          (
            Current == '.' && !is_float
          ) {

            is_float = true;
            ConsumeChar();

          }

          else [[
            /* nullAttr */
          ]] break;

        }

        Type = is_float ? Token::TokenType::FloatLiteral
                        : Token::TokenType::IntLiteral;

      }

      else if
      (
        std::isalpha
        (
          static_cast<unsigned char>(CurrentChar)
        ) || CurrentChar == '_'
      ) {

        while
        (
          FileOffset < k_Cbuffer.size()
        ) {

          char c = k_Cbuffer[FileOffset];

          if
          (
            std::isalnum
            (
              static_cast<unsigned char>(c)
            ) || c == '_'
          ) ConsumeChar();

          else [[
            /* nullAttr */
          ]] break;

        }

        std::string_view WordText(
          k_Cbuffer.subview(
            TokenStartOffset,
            FileOffset - TokenStartOffset
          )
        );

        Type = FindKeywordOrIdentifier(WordText);

      }

      else [[
        /* nullAttr */
      ]] ConsumeChar();

      if
      (
        Type != Token::TokenType::Unknown
      ) {

        std::string_view TokenText(
          k_Cbuffer.subview(
            TokenStartOffset,
            FileOffset - TokenStartOffset
          )
        );

        handleToken
        (
          Token::TokenData
          {
            .Type = Type,
            .Sv_Val_ = TokenText,
            .Lexer_Size_t_Line_ = FileLine,
            .Lexer_Size_t_Column_ = FileColumn,
            .Lexer_Size_t_Offset_ = FileOffset
          }
        );

      }

    }

    handleToken
    (
      Token::TokenData
      {
        .Type = Token::TokenType::EndOfFile,
        .Sv_Val_ = "",
        .Lexer_Size_t_Line_ = FileLine,
        .Lexer_Size_t_Column_ = FileColumn,
        .Lexer_Size_t_Offset_ = FileOffset
      }
    );

  }

}
