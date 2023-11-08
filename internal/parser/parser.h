#pragma once
#include <unordered_map>
#include <vector>
#include "lexer.h"

// Tokens
// LineComment    -> // .*\\n

// LogicalOp      -> || | &&
// RelativeOp     -> < | > | <= | >= | == | !=
// ArithmeticOp   -> + | - | / | *
// UnaryPrefix    -> !

// Digit          -> [0-9]
// IntegerLiteral -> Digit+ | -Digit+
// FloatLiteral   -> IntegerLiteral . Digit+
// NumLiteral     -> IntegerLiteral | FloatLiteral

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


// Grammar
// BasicRValue    -> Identifier | CharLiteral | StringLiteral | BooleanLiteral | FloatLiteral | IntegerLiteral

// BoolExpr       -> LogicalExpr
// LogicalExpr    -> RelativeExpr   ( LogicalOp    RelativeExpr   )*
// RelativeExpr   -> ArithmeticExpr ( RelativeOp   ArithmeticExpr )*
// ArithmeticExpr -> PrimaryExpr    ( ArithmeticOp PrimaryExpr    )* | PrimaryStrExpr (+ PrimaryStrExpr)*
// PrimaryExpr    -> ( BoolExpr ) | NumLiteral | BooleanLiteral | Identifier<Int> | Identifier<Float> | Identifier<Bool> | ! BoolExpr<Bool>
// PrimaryStrExpr -> StringLiteral | Identifier<String>

// BaseExpression  -> BoolExpr | Identifier<Char> | CharLiteral
// ArrayExpression -> { ((BaseExpression | ArrayExpr | \Eps) , )* (BaseExpression | ArrayExpr | \Eps) }
// Expression      -> BaseExpression | ArrayExpr

// Declaration    -> BasicType Identifier ; | BasicType Identifier ArrayExt ;
// Assignment     -> Identifier = Expression;

// PrintCall      -> Print \( Expression \) ;
// ReadCall       -> Read \( Expression \) ;

// Statement      -> Declaration | Assignment | PrintCall | ReadCall

// Program        -> Statement* EOF

namespace pa {
        class Parser {
                enum class Operator {
                        And,
                        Not,
                        Or,
                        
                };
                
        public: // Static Data
                struct SymbolData {
                        TokenType type{TokenType::INVALID};
                        std::vector<size_t> sizes{};
                };
        public: // Constructors/Destructors/Overloads
                Parser(const std::string_view src) : m_source(src), m_lexer(src) {};
        public: // Public Member Functions
                void parseProgram();
                void parseStatement();
                
                void parseDeclaration();
                void parseAssignment();
                void parsePrintCall();
                void parseReadCall();
                
                SymbolData parseBoolExpr();
                SymbolData parseLogicalExpr();
                SymbolData parseComparisonExpr();
                SymbolData parseArithmeticExpr();
                SymbolData parsePrimaryExpr();
                
                SymbolData parseBaseExpression();
                SymbolData parseArrayExpression();
        
                SymbolData parseExpression();
        
        public: // Public Member Variables
        private: // Private Member Functions
                static TokenType deLiteralType(pa::TokenType type);
                void validateAssignment(pa::Token token, SymbolData symbol_data);
                bool identifierIsType(pa::Token token, const SymbolData& symbol_data);
                void consumeOpenParen(std::string_view message);
                void consumeCloseParen(std::string_view message);
                static void error(std::string_view message);
        private: // Private Member Variables
                std::string_view m_source;
                pa::Lexer m_lexer;
                std::unordered_map<std::string, SymbolData> m_symbol_table; // Identifier name -> Type
                std::unordered_map<std::string, std::vector<SymbolData>> m_array_symbol_table;
                int32_t m_parenthesis_depth{0};
        };
}