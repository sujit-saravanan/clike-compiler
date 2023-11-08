#include <vector>
#include <tuple>
#include <algorithm>
#include "parser.h"

#define BasicType Int, TokenType::Bool, TokenType::Float, TokenType::Char, TokenType::String
#define BasicRValue Identifier, TokenType::CharLiteral, TokenType::StringLiteral, TokenType::True, TokenType::False, TokenType::FloatLiteral, TokenType::IntLiteral


template<typename T>
std::string str(T begin, T end) {
        std::stringstream ss;
        for (; begin != end; begin++)
                ss << '[' << *begin << ']';
        return ss.str();
}

bool sizesAreEqual(const std::vector<size_t> &l_sizes, const std::vector<size_t> &r_sizes) {
        for (size_t i = 0; i < l_sizes.size(); i++) {
                if (l_sizes[i] == 0)
                        continue;
                else if (l_sizes[i] != r_sizes[i])
                        return false;
        }
        return true;
}


template<typename T>
std::string reverse_str(T begin, T end) {
        std::stringstream ss;
        for (; end != begin; end--)
                ss << '[' << *(end - 1) << ']';
        return ss.str();
}

void pa::Parser::error(std::string_view message) {
        std::cout << message << "\n";
        std::exit(EXIT_FAILURE);
}


void pa::Parser::parseProgram() {
        while (!m_lexer.is<TokenType::Eof>())
                parseStatement();
        m_lexer.eat<TokenType::Eof>("parseProgram");
}

void pa::Parser::parseStatement() {
        auto tok = m_lexer.peek<TokenType::BasicType, TokenType::Identifier, TokenType::Print, TokenType::Read>("parseStatement");
        
        switch (tok.type) {
                case TokenType::Bool:
                case TokenType::Float:
                case TokenType::Char:
                case TokenType::String:
                case TokenType::Int:
                        parseDeclaration();
                        break;
                
                case TokenType::Identifier:
                        parseAssignment();
                        break;
                
                case TokenType::Print:
                        parsePrintCall();
                        break;
                
                case TokenType::Read:
                        parseReadCall();
                        break;
        }
}

void pa::Parser::parseDeclaration() {
        SymbolData symbol_data;
        
        // Eat and store the type
        symbol_data.type = m_lexer.eat<TokenType::BasicType>("parseDeclaration").type;
        
        // Eat the identifier and store its name
        std::string_view symbol_name = m_lexer.eat<TokenType::Identifier>("parseDeclaration").toString(m_source);
        
        // Eat all the array extension and set the sizes to the sizes of the arrays
        std::vector<size_t> sizes;
        while (m_lexer.is<TokenType::Array>())
                sizes.push_back(std::stoll(std::string(m_lexer.eat<TokenType::Array>("parseDeclaration").toString(m_source))));
        if (sizes.empty())
                symbol_data.sizes = {1};
        else
                symbol_data.sizes = sizes;
        
        // Insert the variable into the symbol table
        m_symbol_table[std::string(symbol_name)] = symbol_data;
        
        
        m_lexer.eat<TokenType::SemiColon>("parseDeclaration");
}

void pa::Parser::parseAssignment() {
        // Eat the identifier and store its name
        auto tok = m_lexer.eat<TokenType::Identifier>("parseAssignment");
        
        // Eat the equals
        m_lexer.eat<TokenType::Equals>("parseAssignment");
        
        // Parse the assigned expression and validate the assignnment
        auto expr_symbol_data = parseExpression();
        std::reverse(expr_symbol_data.sizes.begin(), expr_symbol_data.sizes.end());
        validateAssignment(tok, expr_symbol_data);
        
        // Eat the semicolon
        m_lexer.eat<TokenType::SemiColon>("parseDeclaration");
}

void pa::Parser::parsePrintCall() {
        // Eat the print statement
        m_lexer.eat<TokenType::Print>("parsePrintCall");
        
        // Eat the open paren
        m_lexer.eat<TokenType::OpenParen>("parsePrintCall");
        
        // Validate the parameter
        parseExpression();
        
        // Eat the close paren
        m_lexer.eat<TokenType::CloseParen>("parsePrintCall");
        
        
        // Eat the semicolon
        m_lexer.eat<TokenType::SemiColon>("parsePrintCall");
}

void pa::Parser::parseReadCall() {
        // Eat the print statement
        m_lexer.eat<TokenType::Read>("parseReadCall");
        
        // Eat the open paren
        m_lexer.eat<TokenType::OpenParen>("parseReadCall");
        
        // Validate the parameter
        parseExpression();
        
        // Eat the close paren
        m_lexer.eat<TokenType::CloseParen>("parseReadCall");
        
        
        // Eat the semicolon
        m_lexer.eat<TokenType::SemiColon>("parseReadCall");
}


pa::Parser::SymbolData pa::Parser::parseExpression() {
        auto tok = m_lexer.peek<TokenType::OpenCurly, TokenType::CharLiteral, TokenType::Not, TokenType::Identifier, TokenType::True, TokenType::False, TokenType::IntLiteral, TokenType::FloatLiteral, TokenType::StringLiteral, TokenType::OpenParen>("parseExpression");
        
        if (tok.type == TokenType::OpenCurly)
                // ArrayExpr
                return parseArrayExpression();
        else
                // BaseExpr
                return parseBaseExpression();
}


pa::Parser::SymbolData pa::Parser::parseArrayExpression() {
        Token start = m_lexer.eat<TokenType::OpenCurly>("parseArrayExpression");
        
        size_t size = 0;
        SymbolData symbol_data = {TokenType::INVALID};
        
        auto tok = m_lexer.peek<TokenType::OpenCurly, TokenType::CloseCurly, TokenType::Not, TokenType::CharLiteral, TokenType::Identifier, TokenType::True, TokenType::False, TokenType::IntLiteral, TokenType::FloatLiteral, TokenType::StringLiteral, TokenType::OpenParen>("parseArrayExpression");
        
        while (!m_lexer.is<TokenType::CloseCurly>()) {
                if (tok.type == TokenType::OpenCurly) {
                        // ArrayExpr
                        auto expr_symbol_data = parseArrayExpression();
                        
                        if (symbol_data.type == TokenType::INVALID || symbol_data.type == TokenType::ALL)
                                symbol_data.type = expr_symbol_data.type;
                        
                        if (symbol_data.sizes.empty())
                                for (const auto expr_size: expr_symbol_data.sizes)
                                        symbol_data.sizes.push_back(expr_size);
                        
                        if ((symbol_data.type != expr_symbol_data.type || !sizesAreEqual(symbol_data.sizes, expr_symbol_data.sizes)) && (expr_symbol_data.type != TokenType::ALL && symbol_data.type != TokenType::ALL))
                                error(std::format("Parsing Error(parseArrayExpression {}): Found expression of type {}{} in array literal of type {}{}", start.start + 1, Token::typeToString(expr_symbol_data.type), reverse_str(expr_symbol_data.sizes.begin(), expr_symbol_data.sizes.end()), Token::typeToString(symbol_data.type), str(symbol_data.sizes.begin(), symbol_data.sizes.end())));
                } else {
                        // BaseExpr
                        auto expr_symbol_data = parseBaseExpression();
                        
                        if (symbol_data.type == TokenType::INVALID || symbol_data.type == TokenType::ALL)
                                symbol_data.type = expr_symbol_data.type;
                        else if (symbol_data.type != expr_symbol_data.type && expr_symbol_data.type != TokenType::ALL && symbol_data.type != TokenType::ALL)
                                error(std::format("Parsing Error(parseArrayExpression {}): Found expression of type {} in array literal of type {}", start.start + 1, Token::typeToString(expr_symbol_data.type), Token::typeToString(symbol_data.type)));
                }
                
                size++;
                
                if (!m_lexer.is<TokenType::CloseCurly>())
                        start = m_lexer.eat<TokenType::Comma>("parseArrayExpression");
        }
        
        m_lexer.eat<TokenType::CloseCurly>("parseArrayExpression");
        
        symbol_data.sizes.push_back(size);
        
        if (size == 0) {
                
                return {TokenType::ALL, symbol_data.sizes};
        }
        
        
        return symbol_data;
}

pa::Parser::SymbolData pa::Parser::parseBaseExpression() {
        auto tok = m_lexer.peek<TokenType::Identifier, TokenType::Not, TokenType::CharLiteral, TokenType::True, TokenType::False, TokenType::IntLiteral, TokenType::FloatLiteral, TokenType::StringLiteral, TokenType::OpenParen>("parseBaseExpression");
        switch (tok.type) {
                // BaseExpression -> Identifier<Char> | CharLiteral
                case TokenType::CharLiteral:
                        m_lexer.eat<TokenType::CharLiteral>("parseBaseExpression");
                        return {TokenType::Char, {1}};
                case TokenType::Identifier:
                        if (!m_symbol_table.contains(std::string(tok.toString(m_source))))
                                error(std::format("Parsing Error(parseBaseExpression {}): Variable '{}' assigned to before declaration.", tok.start, tok.toString(m_source)));
                        if (m_symbol_table[std::string(tok.toString(m_source))].type == TokenType::Char) {
                                m_lexer.eat<TokenType::Identifier>("parseBaseExpression");
                                return {TokenType::Char, {1}};
                        }
                                // BaseExpression -> BoolExpr
                        else
                                return parseBoolExpr();
                case TokenType::True:
                case TokenType::False:
                case TokenType::IntLiteral:
                case TokenType::FloatLiteral:
                case TokenType::StringLiteral:
                case TokenType::OpenParen:
                case TokenType::Not:
                        return parseBoolExpr();
                default:
                        error(std::format("How did we get here? parseBaseExpression {}\n", Token::typeToString(tok.type)));
                        return {TokenType::INVALID};
        }
}

pa::Parser::SymbolData pa::Parser::parseBoolExpr() {
        return parseLogicalExpr();
}

// (( 1 + (4 + 1) ) >= (2 + 4)) && False
// parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary
// ( 1 + (4 + 1) ) >= (2 + 4)) && False
// parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary
// 1 + (4 + 1) ) >= (2 + 4)) && False
// parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic
// (4 + 1) ) >= (2 + 4)) && False
// parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary
// 4 + 1) ) >= (2 + 4)) && False
// parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic
// ) ) >= (2 + 4)) && False
// parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic
// ) >= (2 + 4)) && False
// parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic
// >= (2 + 4)) && False
// parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison
// (2 + 4)) && False
// parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary
// 2 + 4)) && False
// parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic
// )) && False
// parseExpression -> parseLogical -> parseComparison -> parseArithmetic -> parsePrimary -> parseExpression -> parseLogical -> parseComparison -> parseArithmetic
// ) && False
// parseExpression -> parseLogical -> parseComparison -> parseArithmetic
// && False
// parseExpression
//

pa::Parser::SymbolData pa::Parser::parseLogicalExpr() {
        auto current_pos = m_lexer.peek().start;
        SymbolData symbol_data = parseComparisonExpr();
        
        while (m_lexer.is<TokenType::And, TokenType::Or>()) {
                if (symbol_data.type == TokenType::String)
                        error(std::format("Parsing Error(parseLogicalExpr {}): Trying to perform logical operation on string.", current_pos));
                
                symbol_data.type = TokenType::Bool;
                m_lexer.eat<TokenType::And, TokenType::Or>("parseLogical");
                
                parseComparisonExpr();
        }
        
        return symbol_data;
}

pa::Parser::SymbolData pa::Parser::parseComparisonExpr() {
        auto current_pos = m_lexer.peek().start;
        SymbolData symbol_data = parseArithmeticExpr();
        
        while (m_lexer.is<TokenType::GreaterThan, TokenType::LessThan, TokenType::GreaterThanOrEquals, TokenType::LessThanOrEquals, TokenType::EqualsEquals, TokenType::NotEquals>()) {
                if (symbol_data.type == TokenType::String)
                        error(std::format("Parsing Error(parseComparisonExpr {}): Trying to perform comparison operation on string.", current_pos));
                
                symbol_data.type = TokenType::Bool;
                m_lexer.eat<TokenType::GreaterThan, TokenType::LessThan, TokenType::GreaterThanOrEquals, TokenType::LessThanOrEquals, TokenType::EqualsEquals, TokenType::NotEquals>("parseComparison");
                
                parseArithmeticExpr();
        }
        
        return symbol_data;
}

pa::Parser::SymbolData pa::Parser::parseArithmeticExpr() {
        auto current_pos = m_lexer.peek().start;
        SymbolData symbol_data = parsePrimaryExpr();
        SymbolData curr_symbol_data = symbol_data;
        
        // Bool            -> Bool
        // Int             -> Int
        // Float           -> Float
        // String          -> String
        // String + String -> String
        
        // Bool op Bool    -> Int
        // Bool op Int     -> Int
        // Int op Int      -> Int
        
        // Bool op Float   -> Float
        // Int op Float    -> Float
        // Float op Float  -> Float
        
        // Turns Bool | Int -> Int on arithmetic operation
        if (m_lexer.is<TokenType::Plus, TokenType::Minus, TokenType::ForwardSlash, TokenType::Asterisk>() && curr_symbol_data.type != TokenType::Float && curr_symbol_data.type != TokenType::String)
                curr_symbol_data.type = TokenType::Int;
        
        while (m_lexer.is<TokenType::Plus, TokenType::Minus, TokenType::ForwardSlash, TokenType::Asterisk>()) {
                // Only allows + for String
                if (curr_symbol_data.type == TokenType::String && !m_lexer.is<TokenType::Plus>())
                        error(std::format("Parsing Error(parseArithmeticExpr {}): Trying to perform non-plus arithmetic operation on string.", current_pos));
                
                m_lexer.eat<TokenType::Plus, TokenType::Minus, TokenType::ForwardSlash, TokenType::Asterisk>("parseArithmetic");
                SymbolData rhs_symbol_data = parsePrimaryExpr();
                
                // Only allows for String + String
                if (curr_symbol_data.type == TokenType::String && rhs_symbol_data.type != TokenType::String)
                        error(std::format("Parsing Error(parseArithmeticExpr {}): Trying to append non-string to string.", current_pos));
                
                // Turns expression into float if a single float is encountered
                if (rhs_symbol_data.type == TokenType::Float)
                        symbol_data.type = TokenType::Float;
                
                curr_symbol_data = rhs_symbol_data;
        }
        
        return symbol_data;
}

pa::Parser::SymbolData pa::Parser::parsePrimaryExpr() {
        if (m_lexer.is<TokenType::Not>()) {
                auto not_token = m_lexer.eat<TokenType::Not>("parsePrimary");
                if (!m_lexer.is<TokenType::Identifier, TokenType::True, TokenType::False, TokenType::OpenParen>())
                        error(std::format("Parsing Error(parsePrimaryExpr {}): Performing Not operation on type {} is not valid", not_token.start, Token::typeToString((m_lexer.eat().type))));
                
                if (m_lexer.is<TokenType::Identifier>()) {
                        auto possible_boolean_identifier_token = m_lexer.peek<TokenType::Identifier>("parsePrimary");
                        if (possible_boolean_identifier_token.type != TokenType::Bool) {
                                error(std::format("Parsing Error(parsePrimaryExpr {}): Performing Not operation on type {} is not valid", not_token.start, Token::typeToString((m_lexer.eat().type))));
                        } else {
                                m_lexer.eat<TokenType::Identifier>("parsePrimary");
                                return {TokenType::Bool, {1}};
                        }
                } else if (m_lexer.is<TokenType::True, TokenType::False>()){
                        m_lexer.eat<TokenType::True, TokenType::False>("parsePrimary");
                        return {TokenType::Bool, {1}};
                } else {
                        consumeOpenParen("parsePrimary");
                        auto expr_tok = parseBaseExpression();
                        consumeCloseParen("parsePrimary");
                        if (expr_tok.type != TokenType::Bool)
                                error(std::format("Parsing Error(parsePrimaryExpr {}): Performing Not operation on type {} is not valid", not_token.start, Token::typeToString((expr_tok.type))));
                        else
                                return {TokenType::Bool, {1}};
                        
                }

        }
        
        
        auto tok = m_lexer.peek<TokenType::OpenParen, TokenType::Identifier, TokenType::IntLiteral, TokenType::FloatLiteral, TokenType::True, TokenType::False, TokenType::StringLiteral>("parsePrimary");
        
        SymbolData symbol_data = {TokenType::INVALID, {1}};
        
        switch (tok.type) {
                // Identifier<Int> | Identifier<Float> | Identifier<Bool>
                case TokenType::Identifier:
                        m_lexer.eat<TokenType::Identifier>("parsePrimary");
                        
                        if (!m_symbol_table.contains(std::string(tok.toString(m_source))))
                                error(std::format("Parsing Error(parsePrimaryExpr {}): Variable '{}' assigned to before declaration.", tok.start, tok.toString(m_source)));
                        
                        symbol_data = m_symbol_table[std::string(tok.toString(m_source))];
                        
                        if (symbol_data.type != TokenType::Int && symbol_data.type != TokenType::Float && symbol_data.type != TokenType::Bool && symbol_data.type != TokenType::String)
                                error(std::format("Parsing Error(parsePrimaryExpr {}): Expected variable of type Int | Float | Bool | String, got type {} instead.", tok.start, Token::typeToString(symbol_data.type)));
                        
                        break;
                        
                        // ( Expression )
                case TokenType::OpenParen:
                        consumeOpenParen("parsePrimary");
                        symbol_data = parseBaseExpression();
                        consumeCloseParen("parsePrimary");
                        break;
                        
                        
                        // NumLiteral
                case TokenType::IntLiteral:
                        m_lexer.eat<TokenType::IntLiteral>("parsePrimary");
                        symbol_data = {TokenType::Int, {1}};
                        break;
                case TokenType::FloatLiteral:
                        m_lexer.eat<TokenType::FloatLiteral>("parsePrimary");
                        symbol_data = {TokenType::Float, {1}};
                        break;
                case TokenType::StringLiteral:
                        m_lexer.eat<TokenType::StringLiteral>("parsePrimary");
                        symbol_data = {TokenType::String, {1}};
                        break;
                        
                        // BooleanLiteral
                case TokenType::True:
                case TokenType::False:
                        m_lexer.eat<TokenType::True, TokenType::False>("parsePrimary");
                        symbol_data = {TokenType::Bool, {1}};
                        break;
        }
        
        return symbol_data;
}


void pa::Parser::consumeOpenParen(std::string_view message) {
        m_lexer.eatIfTokenIs<TokenType::OpenParen>(std::string(message) + " -> consumeOpenParen");
        m_parenthesis_depth++;
}
void pa::Parser::consumeCloseParen(std::string_view message) {
        m_lexer.eatIfTokenIs<TokenType::CloseParen>(std::string(message) + " -> consumeCloseParen");
        m_parenthesis_depth--;
}


// Turns a literal type into a regular type (regular types passed in return themselves)
pa::TokenType pa::Parser::deLiteralType(pa::TokenType type) {
        switch (type) {
                case TokenType::IntLiteral:
                case TokenType::Int:
                        return TokenType::Int;
                case TokenType::Bool:
                case TokenType::True:
                case TokenType::False:
                        return TokenType::Bool;
                case TokenType::Float:
                case TokenType::FloatLiteral:
                        return TokenType::Float;
                case TokenType::Char:
                case TokenType::CharLiteral:
                        return TokenType::Char;
                case TokenType::String:
                case TokenType::StringLiteral:
                        return TokenType::String;
                
                case TokenType::IntArray:
                        return TokenType::IntArray;
                case TokenType::BoolArray:
                        return TokenType::BoolArray;
                case TokenType::FloatArray:
                        return TokenType::FloatArray;
                case TokenType::CharArray:
                        return TokenType::CharArray;
                case TokenType::StringArray:
                        return TokenType::StringArray;
                
                default:
                        return TokenType::INVALID;
        }
}

void pa::Parser::validateAssignment(pa::Token token, pa::Parser::SymbolData symbol_data) {
        if (!m_symbol_table.contains(std::string(token.toString(m_source))))
                error(std::format("Parsing Error(validateAssignment {}): Variable '{}' assigned to before declaration.", token.start, token.toString(m_source)));
        
        auto target_symbol_data = m_symbol_table.at(std::string(token.toString(m_source)));
        
        if ((target_symbol_data.type != deLiteralType(symbol_data.type) || !sizesAreEqual(target_symbol_data.sizes, symbol_data.sizes)) && symbol_data.type != TokenType::ALL) {
                error(std::format("Parsing Error(validateAssignment {}): R-value of type ({}, {}) assigned to variable '{}' of type ({}, {}).",
                                  token.start,
                                  Token::typeToString(deLiteralType(symbol_data.type)),
                                  str(symbol_data.sizes.begin(), symbol_data.sizes.end()),
                                  token.toString(m_source),
                                  Token::typeToString(target_symbol_data.type),
                                  str(target_symbol_data.sizes.begin(), target_symbol_data.sizes.end())));
        }
}
bool pa::Parser::identifierIsType(pa::Token token, const pa::Parser::SymbolData& symbol_data) {
        if (!m_symbol_table.contains(std::string(token.toString(m_source))))
                return false;
        
        auto target_symbol_data = m_symbol_table.at(std::string(token.toString(m_source)));
        
        if (target_symbol_data.type != deLiteralType(symbol_data.type) || target_symbol_data.sizes != symbol_data.sizes)
                return false;
        
        return true;
}
