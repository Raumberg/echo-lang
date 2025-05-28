#ifndef LEXER_H
#define LEXER_H

#include <stdbool.h>
#include <stddef.h>

// Token types
typedef enum {
    TOKEN_KEYWORD,
    TOKEN_IDENTIFIER,
    TOKEN_INTEGER,
    TOKEN_FLOAT,
    TOKEN_STRING,
    TOKEN_CHAR,
    TOKEN_OPERATOR,
    TOKEN_DELIMITER,
    TOKEN_PREPROCESSOR,
    TOKEN_EOF,
    TOKEN_ERROR
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char* value;
    int line;
    int column;
    int length;
} Token;

// Lexer structure
typedef struct {
    const char* source;
    size_t position;
    size_t length;
    int line;
    int column;
    char current_char;
} Lexer;

// Function declarations
Lexer* lexer_create(const char* source_code);
void lexer_destroy(Lexer* lexer);

Token lexer_next_token(Lexer* lexer);
void token_destroy(Token* token);

// Utility functions
bool is_keyword(const char* str);
bool is_alpha(char c);
bool is_digit(char c);
bool is_alnum(char c);
bool is_whitespace(char c);
bool is_operator_char(char c);
bool is_delimiter_char(char c);

// Internal functions
void lexer_advance(Lexer* lexer);
char lexer_peek(Lexer* lexer);
void lexer_skip_whitespace(Lexer* lexer);
void lexer_skip_comment(Lexer* lexer);

Token lexer_read_string(Lexer* lexer);
Token lexer_read_char(Lexer* lexer);
Token lexer_read_number(Lexer* lexer);
Token lexer_read_identifier(Lexer* lexer);
Token lexer_read_operator(Lexer* lexer);
Token lexer_read_preprocessor(Lexer* lexer);

Token token_create(TokenType type, const char* value, int line, int column);
Token token_error(const char* message, int line, int column);

#endif // LEXER_H 