#pragma once
#include <cstddef>
#include <string_view>
#include <iostream>

namespace pa {
        enum class TokenType {
                // Keywords
                Int,
                Bool,
                Float,
                Char,
                String,
                
                // Misc
                Num,
                Identifier,
                Print,
                Read,
                Equals,
                
                // Literals
                CharLiteral,
                StringLiteral,
                IntLiteral,
                FloatLiteral,
                True,
                False,
                
                // Array
                Array,
                IntArray,
                BoolArray,
                FloatArray,
                CharArray,
                StringArray,
                
                // Grouping
                OpenParen,
                CloseParen,
                OpenCurly,
                CloseCurly,
                Comma,
                
                // Language
                SemiColon,
                
                // Arithmetic
                Plus,
                Minus,
                Asterisk,
                ForwardSlash,
                
                // Relational
                LessThan,
                LessThanOrEquals,
                GreaterThan,
                GreaterThanOrEquals,
                EqualsEquals,
                NotEquals,
                
                // Logical
                And,
                Or,
                
                // Unary
                Not,
                
                Eof,
                INVALID,
                ALL,
        };
        
        struct Token {
                TokenType type{pa::TokenType::INVALID};
                size_t start{0};
                size_t end{0};
                
                static constexpr std::string_view typeToString(TokenType token_type) {
                        switch (token_type) {
                                // Keywords
                                case TokenType::Int:
                                        return "Int";
                                case TokenType::Bool:
                                        return "Bool";
                                case TokenType::Float:
                                        return "Float";
                                case TokenType::Char:
                                        return "Char";
                                case TokenType::String:
                                        return "String";
                                case TokenType::True:
                                        return "True";
                                case TokenType::False:
                                        return "False";
                                
                                // Misc
                                case TokenType::Num:
                                        return "Num";
                                case TokenType::Identifier:
                                        return "Identifier";
                                case TokenType::Print:
                                        return "Print";
                                case TokenType::Read:
                                        return "Read";
                                case TokenType::Equals:
                                        return "Equals";
                                
                                // Literals
                                case TokenType::CharLiteral:
                                        return "CharLiteral";
                                case TokenType::StringLiteral:
                                        return "StringLiteral";
                                case TokenType::IntLiteral:
                                        return "IntLiteral";
                                case TokenType::FloatLiteral:
                                        return "FloatLiteral";
                                
                                // Array
                                case TokenType::Array:
                                        return "Array";
                                case TokenType::IntArray:
                                        return "IntArray";
                                case TokenType::BoolArray:
                                        return "BoolArray";
                                case TokenType::FloatArray:
                                        return "FloatArray";
                                case TokenType::CharArray:
                                        return "CharArray";
                                case TokenType::StringArray:
                                        return "StringArray";
                                
                                // Grouping
                                case TokenType::OpenParen:
                                        return "OpenParen";
                                case TokenType::CloseParen:
                                        return "CloseParen";
                                case TokenType::OpenCurly:
                                        return "OpenCurly";
                                case TokenType::CloseCurly:
                                        return "CloseCurly";
                                case TokenType::Comma:
                                        return "Comma";
                                
                                // Language
                                case TokenType::SemiColon:
                                        return "SemiColon";
                                
                                // Arithmetic
                                case TokenType::Plus:
                                        return "Plus";
                                case TokenType::Minus:
                                        return "Minus";
                                case TokenType::Asterisk:
                                        return "Asterisk";
                                case TokenType::ForwardSlash:
                                        return "ForwardSlash";
                                
                                // Relational
                                case TokenType::LessThan:
                                        return "LessThan";
                                case TokenType::LessThanOrEquals:
                                        return "LessThanOrEquals";
                                case TokenType::GreaterThan:
                                        return "GreaterThan";
                                case TokenType::GreaterThanOrEquals:
                                        return "GreaterThanOrEquals";
                                case TokenType::EqualsEquals:
                                        return "EqualsEquals";
                                case TokenType::NotEquals:
                                        return "NotEquals";
                                
                                // Logical
                                case TokenType::And:
                                        return "And";
                                case TokenType::Or:
                                        return "Or";
                                case TokenType::Not:
                                        return "Not";
                                
                                case TokenType::Eof:
                                        return "Eof";
                                case TokenType::INVALID:
                                        return "INVALID";
                                default:
                                        return "UNKNOWN";
                        }
                }
                
                [[nodiscard]] std::string_view toString(std::string_view src) const {
                        return {src.begin() + start, end - start + 1};
                }
                
                void print(std::string_view src) const {
                        std::cout << typeToString(type) << ": " << std::string_view(src.begin() + start, end - start + 1) << "\n";
                }
        };
}