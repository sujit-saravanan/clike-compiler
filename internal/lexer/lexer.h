#pragma once
#include <string_view>
#include <ostream>
#include <format>
#include <sstream>
#include "token.h"

// Tokens
// LineComment    -> // .*\n

// LogicalOp      -> || | &&
// ArithmeticOp   -> + | - | / | *
// RelativeOp     -> < | > | <= | >= | == | !=
// UnaryPrefix    -> !

// Digit          -> [0-9]
// IntegerLiteral -> Digit+ | -Digit+
// FloatLiteral   -> IntegerLiteral . Digit+

// Letter         -> [a-z | A-Z]

// SafeAscii      -> All Ascii characters that don't require an escape
// UnsafeAscii    -> All Ascii characters that require an escape
// ValidAscii     -> (SafeAscii | \\ UnsafeAscii)+

// CharLiteral    -> ' ValidAscii '
// StringLiteral  -> " ValidAscii* "
// BooleanLiteral -> True | False;

// Identifier     -> (Letter | _) (Letter | _ | Digit)*
// BasicType      -> int | bool | float | char | string

// ArrayExt       -> [ IntegerLiteral ]

// Print          -> print
// Read           -> read

namespace pa {
        class Lexer {
        public: // Static Data
        public: // Constructors/Destructors/Overloads
                Lexer(std::string_view src);
        public: // Public Member Functions
                Token peek();
                Token peek_prev();
                Token eat();
                
                // Prints all values passed into the fold expression with a delimeter | in between
                template<pa::TokenType... ExpectedTypes>
                static constexpr std::string toString() {
                        std::ostringstream oss;
                        bool first = true;
                        ((oss << (first ? "" : " | ") << pa::Token::typeToString(ExpectedTypes), first = false), ...);
                        return oss.str();
                }
                
                // Check if the passed type matches any of the ExpectedTypes using fold expression
                template<pa::TokenType... ExpectedTypes>
                constexpr inline void expect(std::string_view rule_name) {
                        if (!((m_current_lexed.type == ExpectedTypes) || ...)) {
                                error(std::format("Parsing Error({}, {}): Expected token types ( {} ), got {}({}) instead.",
                                                  m_current_lexed.start,
                                                  rule_name,
                                                  toString<ExpectedTypes...>(),
                                                  pa::Token::typeToString(m_current_lexed.type),
                                                  m_current_lexed.toString(m_source)
                                      )
                                );
                        }
                }
                
                template<pa::TokenType... ExpectedTypes>
                pa::Token peek(std::string_view rule_name) {
                        expect<ExpectedTypes...>(rule_name);
                        return peek();
                }
                
                template<pa::TokenType... ExpectedTypes>
                pa::Token eat(std::string_view rule_name) {
                        expect<ExpectedTypes...>(rule_name);
                        return eat();
                }
                
                template<pa::TokenType... ExpectedTypes>
                bool is() {
                        return ((m_current_lexed.type == ExpectedTypes) || ...);
                }
                
                template<pa::TokenType... ExpectedTypes>
                void eatIfTokenIs(std::string_view rule_name) {
                        is<ExpectedTypes...>();
                        eat<ExpectedTypes...>(rule_name);
                }
                
        public: // Public Member Variables
        private: // Private Member Functions
                void getNextToken();
                void validateAndIncrementIndex(std::string_view message);
                void incrementIndex();
                
                char currentCharacter();
                Token lexNum();
                Token lexChar();
                Token lexString();
                Token lexWord();
                Token lexArrayExt();
                
                static void error(std::string_view message);
        private: // Private Member Variables
                std::string_view m_source;
                Token m_current_lexed;
                Token m_prev_lexed;
                size_t m_current_index{0};
        };
}