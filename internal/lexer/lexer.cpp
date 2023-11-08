#include <iostream>
#include "lexer.h"

pa::Lexer::Lexer(const std::string_view src) : m_source(src) {
        getNextToken();
}



pa::Token pa::Lexer::peek() {
        return m_current_lexed;
}
pa::Token pa::Lexer::eat() {
        m_prev_lexed = m_current_lexed;
        getNextToken();
        return m_prev_lexed;
}
pa::Token pa::Lexer::peek_prev() {
        return m_prev_lexed;
}

void pa::Lexer::getNextToken() {
        while (m_current_index < m_source.size() && std::isspace(currentCharacter()))
                incrementIndex();
        
        if (m_current_index >= m_source.size()) {
                m_current_lexed = {TokenType::Eof, m_current_index, m_current_index};
                return;
        }
        
        if (std::isdigit(currentCharacter()) || currentCharacter() == '-' || currentCharacter() == '.')
                m_current_lexed = lexNum();
        
        else if (std::isalpha(currentCharacter()) || currentCharacter() == '_')
                m_current_lexed = lexWord();
        
        else {
                switch (currentCharacter()) {
                        case '\"':
                                m_current_lexed = lexString();
                                break;
                        case '\'':
                                m_current_lexed = lexChar();
                                break;
                        case '[':
                                m_current_lexed = lexArrayExt();
                                break;
                        case '(':
                                m_current_lexed = {TokenType::OpenParen, m_current_index, m_current_index};
                                m_current_index++;
                                break;
                        case ')':
                                m_current_lexed = {TokenType::CloseParen, m_current_index, m_current_index};
                                m_current_index++;
                                break;
                        case '{':
                                m_current_lexed = {TokenType::OpenCurly, m_current_index, m_current_index};
                                m_current_index++;
                                break;
                        case '}':
                                m_current_lexed = {TokenType::CloseCurly, m_current_index, m_current_index};
                                m_current_index++;
                                break;
                        case ',':
                                m_current_lexed = {TokenType::Comma, m_current_index, m_current_index};
                                m_current_index++;
                                break;
                        case ';':
                                m_current_lexed = {TokenType::SemiColon, m_current_index, m_current_index};
                                m_current_index++;
                                break;
                        case '+':
                                m_current_lexed = {TokenType::Plus, m_current_index, m_current_index};
                                m_current_index++;
                                break;
                        case '-':
                                m_current_lexed = {TokenType::Minus, m_current_index, m_current_index};
                                m_current_index++;
                                break;
                        case '*':
                                m_current_lexed = {TokenType::Asterisk, m_current_index, m_current_index};
                                m_current_index++;
                                break;
                        case '/':
                                m_current_index++;
                                if (currentCharacter() == '/') { // //
                                        while (currentCharacter() != '\n' && currentCharacter() != '\r')
                                                m_current_index++;
                                        getNextToken();
                                } else {
                                        m_current_lexed = {TokenType::ForwardSlash, m_current_index - 1, m_current_index - 1};
                                }
                                break;
                        
                        case '<':
                                m_current_index++;
                                if (currentCharacter() == '=') { // <=
                                        m_current_index++;
                                        m_current_lexed = {TokenType::LessThanOrEquals, m_current_index - 2, m_current_index};
                                } else { // <
                                        m_current_lexed = {TokenType::LessThan, m_current_index - 1, m_current_index - 1};
                                }
                                break;
                        case '>':
                                m_current_index++;
                                if (currentCharacter() == '=') { // >=
                                        m_current_index++;
                                        m_current_lexed = {TokenType::GreaterThanOrEquals, m_current_index - 2, m_current_index};
                                } else { // >
                                        m_current_lexed = {TokenType::GreaterThan, m_current_index - 1, m_current_index - 1};
                                }
                                break;
                        case '=':
                                m_current_index++;
                                if (currentCharacter() == '=') { // ==
                                        m_current_index++;
                                        m_current_lexed = {TokenType::EqualsEquals, m_current_index - 2, m_current_index};
                                } else { // =
                                        m_current_lexed = {TokenType::Equals, m_current_index - 1, m_current_index - 1};
                                }
                                break;
                        case '!':
                                m_current_index++;
                                if (currentCharacter() == '=') { // !=
                                        m_current_index++;
                                        m_current_lexed = {TokenType::NotEquals, m_current_index - 2, m_current_index};
                                } else { // !
                                        m_current_lexed = {TokenType::Not, m_current_index - 1, m_current_index - 1};
                                }
                                break;
                        case '&':
                                m_current_index++;
                                if (currentCharacter() == '&') { // &&
                                        m_current_index++;
                                        m_current_lexed = {TokenType::And, m_current_index - 2, m_current_index};
                                } else {
                                        error(std::format("Lexing Error({}): Bitwise operations are not supported, expected sequention &, got {} instead.", m_current_index, currentCharacter()));
                                }
                                break;
                        case '|':
                                m_current_index++;
                                if (currentCharacter() == '|') { // ||
                                        m_current_index++;
                                        m_current_lexed = {TokenType::Or, m_current_index - 2, m_current_index};
                                } else {
                                        error(std::format("Lexing Error({}): Bitwise operations are not supported, expected sequention |, got {} instead.", m_current_index, currentCharacter()));
                                }
                                break;
                        default:
                                error(std::format("Lexing Error({}): Unexpected Character {}.", m_current_index, currentCharacter()));
                                break;
                }
        }
}

pa::Token pa::Lexer::lexNum() {
        TokenType float_or_int = TokenType::IntLiteral;
        bool numbers_at_start = false;
        
        auto start = m_current_index;
        
        if (currentCharacter() == '-')
                m_current_index++;
        
        if (std::isdigit(currentCharacter()))
                numbers_at_start = true;
        
        while (m_current_index < m_source.size() && std::isdigit(currentCharacter()))
                incrementIndex();
        
        if (currentCharacter() == '.') {
                float_or_int = TokenType::FloatLiteral;
                incrementIndex();
        }
        
        if (!std::isdigit(currentCharacter()) && !numbers_at_start)
                error(std::format("Lexing Error({}): Floating point numbers require atleast one digit on atleast one side of the decimal point.", m_current_index, currentCharacter()));
        
        while (m_current_index < m_source.size() && std::isdigit(currentCharacter()))
                incrementIndex();
        
        return {float_or_int, start, m_current_index - 1};
}

pa::Token pa::Lexer::lexChar() {
        auto start = m_current_index;
        validateAndIncrementIndex(std::format("Lexing Error({}): Could not locate closing quote for open quote.", start)); // Get past the starting quote
        
        if (currentCharacter() == '\\')
                validateAndIncrementIndex(std::format("Lexing Error({}): Could not locate closing quote for open quote.", start)); // Eat escape sequence
        else if (currentCharacter() == '\'')
                error(std::format("Lexing Error({}): Empty char.", start)); // Eat escape sequence
        
        validateAndIncrementIndex(std::format("Lexing Error({}): Could not locate closing quote for open quote.", start)); // Get past char
        
        if (currentCharacter() != '\'')// Validate closing quote
                error(std::format("Lexing Error({}): Expected closing quote, found {} instead.", m_current_index, currentCharacter()));
        incrementIndex(); // Get past the closing quote
        
        return {TokenType::CharLiteral, start, m_current_index - 1};
}

pa::Token pa::Lexer::lexString() {
        auto start = m_current_index;
        validateAndIncrementIndex(std::format("Lexing Error({}): Could not locate closing quote for open quote.", start));
        
        while (currentCharacter() != '"') {
                if (currentCharacter() == '\\')
                        validateAndIncrementIndex(std::format("Lexing Error({}): Could not locate closing quote for open quote.", start));
                validateAndIncrementIndex(std::format("Lexing Error({}): Could not locate closing quote for open quote.", start));
        }
        incrementIndex();
        
        
        return {TokenType::StringLiteral, start, m_current_index - 1};
}

pa::Token pa::Lexer::lexArrayExt() {
        validateAndIncrementIndex(std::format("Lexing Error({}): Could not locate closing brace for open brace.", m_current_index)); // Get past the opening brace
        
        Token ret = lexNum(); // Get Size, storing this makes type validation a bit easier
        
        if (currentCharacter() != ']')
                error(std::format("Lexing Error({}): Expected closing brace, found {} instead.", m_current_index, currentCharacter()));
        incrementIndex(); // Get past the closing brace
        
        if (ret.type == TokenType::FloatLiteral)
                error(std::format("Lexing Error({}): Expected Integer Literal, found Float Literal({}).", m_current_index, ret.toString(m_source)));
        
        ret.type = TokenType::Array;
        return ret;
}

static constexpr inline unsigned int hash(std::string_view str) {
        unsigned int hash = 5381;
        for (char c: str)
                hash = ((hash << 5) + hash) + static_cast<unsigned int>(c);
        return hash;
}

static constexpr inline unsigned int operator "" _(char const* chr, size_t) noexcept { return hash(chr); }

pa::Token pa::Lexer::lexWord() {
        auto start = m_current_index;
        while (m_current_index < m_source.size() && (std::isalnum(currentCharacter()) || currentCharacter() == '_'))
                incrementIndex();
        
        switch (hash(std::string_view(m_source.begin() + start, m_current_index - start))) {
                case "True"_:
                        return {TokenType::True, start, m_current_index - 1};
                case "False"_:
                        return {TokenType::False, start, m_current_index - 1};
                
                case "int"_:
                        return {TokenType::Int, start, m_current_index - 1};
                case "bool"_:
                        return {TokenType::Bool, start, m_current_index - 1};
                case "float"_:
                        return {TokenType::Float, start, m_current_index - 1};
                case "char"_:
                        return {TokenType::Char, start, m_current_index - 1};
                case "string"_:
                        return {TokenType::String, start, m_current_index - 1};
                
                case "print"_:
                        return {TokenType::Print, start, m_current_index - 1};
                case "read"_:
                        return {TokenType::Read, start, m_current_index - 1};
                
                default:
                        return {TokenType::Identifier, start, m_current_index - 1};
        }
}

char pa::Lexer::currentCharacter() {
        return m_source[m_current_index];
}

void pa::Lexer::incrementIndex() {
        m_current_index++;
}

void pa::Lexer::validateAndIncrementIndex(const std::string_view message) {
        incrementIndex();
        if (m_current_index >= m_source.size())
                error(message);
}
void pa::Lexer::error(const std::string_view message) {
        std::cout << message << "\n";
        std::exit(EXIT_FAILURE);
}
