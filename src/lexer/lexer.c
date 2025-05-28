#include "lexer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>

// Keywords array
static const char* keywords[] = {
    "fn", "struct", "enum", "if", "else", "for", "while", "return", 
    "break", "continue", "auto", "null", "true", "false", "alloc", 
    "delete", "sizeof", "const", "static", "global", "typedef", 
    "switch", "case", "default",
    // Types
    "i8", "i16", "i32", "i64", "f32", "f64", "bool", "string", "char", "void",
    NULL
};

// Create lexer
Lexer* lexer_create(const char* source_code) {
    if (!source_code) return NULL;
    
    Lexer* lexer = malloc(sizeof(Lexer));
    if (!lexer) return NULL;
    
    lexer->source = source_code;
    lexer->position = 0;
    lexer->length = strlen(source_code);
    lexer->line = 1;
    lexer->column = 1;
    lexer->current_char = lexer->length > 0 ? source_code[0] : '\0';
    
    return lexer;
}

// Destroy lexer
void lexer_destroy(Lexer* lexer) {
    if (lexer) {
        free(lexer);
    }
}

// Advance to next character
void lexer_advance(Lexer* lexer) {
    if (!lexer || lexer->position >= lexer->length) {
        if (lexer) lexer->current_char = '\0';
        return;
    }
    
    if (lexer->current_char == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }
    
    lexer->position++;
    lexer->current_char = (lexer->position < lexer->length) ? 
                         lexer->source[lexer->position] : '\0';
}

// Peek next character
char lexer_peek(Lexer* lexer) {
    if (!lexer || lexer->position + 1 >= lexer->length) {
        return '\0';
    }
    return lexer->source[lexer->position + 1];
}

// Check if string is keyword
bool is_keyword(const char* str) {
    for (int i = 0; keywords[i] != NULL; i++) {
        if (strcmp(str, keywords[i]) == 0) {
            return true;
        }
    }
    return false;
}

// Character classification functions
bool is_alpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool is_digit(char c) {
    return c >= '0' && c <= '9';
}

bool is_alnum(char c) {
    return is_alpha(c) || is_digit(c);
}

bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n' || c == '\r';
}

bool is_operator_char(char c) {
    return strchr("+-*/%=!<>&|^~?:", c) != NULL;
}

bool is_delimiter_char(char c) {
    return strchr("(){}[];,.", c) != NULL;
}

// Skip whitespace
void lexer_skip_whitespace(Lexer* lexer) {
    while (lexer->current_char != '\0' && is_whitespace(lexer->current_char)) {
        lexer_advance(lexer);
    }
}

// Skip comments
void lexer_skip_comment(Lexer* lexer) {
    if (lexer->current_char == '/' && lexer_peek(lexer) == '/') {
        // Single line comment
        while (lexer->current_char != '\0' && lexer->current_char != '\n') {
            lexer_advance(lexer);
        }
    } else if (lexer->current_char == '/' && lexer_peek(lexer) == '*') {
        // Multi line comment
        lexer_advance(lexer); // skip '/'
        lexer_advance(lexer); // skip '*'
        
        while (lexer->current_char != '\0') {
            if (lexer->current_char == '*' && lexer_peek(lexer) == '/') {
                lexer_advance(lexer); // skip '*'
                lexer_advance(lexer); // skip '/'
                break;
            }
            lexer_advance(lexer);
        }
    }
}

// Create token
Token token_create(TokenType type, const char* value, int line, int column) {
    Token token;
    token.type = type;
    token.line = line;
    token.column = column;
    token.length = value ? strlen(value) : 0;
    
    if (value) {
        token.value = malloc(strlen(value) + 1);
        strcpy(token.value, value);
    } else {
        token.value = NULL;
    }
    
    return token;
}

// Create error token
Token token_error(const char* message, int line, int column) {
    return token_create(TOKEN_ERROR, message, line, column);
}

// Destroy token
void token_destroy(Token* token) {
    if (token && token->value) {
        free(token->value);
        token->value = NULL;
    }
}

// Read identifier or keyword
Token lexer_read_identifier(Lexer* lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    char buffer[256];
    int i = 0;
    
    while (lexer->current_char != '\0' && is_alnum(lexer->current_char) && i < 255) {
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);
    }
    buffer[i] = '\0';
    
    TokenType type = is_keyword(buffer) ? TOKEN_KEYWORD : TOKEN_IDENTIFIER;
    return token_create(type, buffer, start_line, start_column);
}

// Read number (integer or float)
Token lexer_read_number(Lexer* lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    char buffer[64];
    int i = 0;
    bool is_float = false;
    
    // Handle hex, binary, octal prefixes
    if (lexer->current_char == '0' && i < 63) {
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);
        
        if (lexer->current_char == 'x' || lexer->current_char == 'X') {
            // Hexadecimal
            buffer[i++] = lexer->current_char;
            lexer_advance(lexer);
            while (lexer->current_char != '\0' && 
                   (is_digit(lexer->current_char) || 
                    (lexer->current_char >= 'a' && lexer->current_char <= 'f') ||
                    (lexer->current_char >= 'A' && lexer->current_char <= 'F')) && i < 63) {
                buffer[i++] = lexer->current_char;
                lexer_advance(lexer);
            }
        } else if (lexer->current_char == 'b' || lexer->current_char == 'B') {
            // Binary
            buffer[i++] = lexer->current_char;
            lexer_advance(lexer);
            while (lexer->current_char != '\0' && 
                   (lexer->current_char == '0' || lexer->current_char == '1') && i < 63) {
                buffer[i++] = lexer->current_char;
                lexer_advance(lexer);
            }
        } else if (lexer->current_char == 'o' || lexer->current_char == 'O') {
            // Octal
            buffer[i++] = lexer->current_char;
            lexer_advance(lexer);
            while (lexer->current_char != '\0' && 
                   (lexer->current_char >= '0' && lexer->current_char <= '7') && i < 63) {
                buffer[i++] = lexer->current_char;
                lexer_advance(lexer);
            }
        }
    }
    
    // Read decimal digits
    while (lexer->current_char != '\0' && is_digit(lexer->current_char) && i < 63) {
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);
    }
    
    // Check for decimal point
    if (lexer->current_char == '.' && is_digit(lexer_peek(lexer)) && i < 63) {
        is_float = true;
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);
        
        while (lexer->current_char != '\0' && is_digit(lexer->current_char) && i < 63) {
            buffer[i++] = lexer->current_char;
            lexer_advance(lexer);
        }
    }
    
    // Check for scientific notation
    if ((lexer->current_char == 'e' || lexer->current_char == 'E') && i < 63) {
        is_float = true;
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);
        
        if (lexer->current_char == '+' || lexer->current_char == '-') {
            buffer[i++] = lexer->current_char;
            lexer_advance(lexer);
        }
        
        while (lexer->current_char != '\0' && is_digit(lexer->current_char) && i < 63) {
            buffer[i++] = lexer->current_char;
            lexer_advance(lexer);
        }
    }
    
    // Check for float suffix
    if (lexer->current_char == 'f' || lexer->current_char == 'F') {
        is_float = true;
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);
    }
    
    buffer[i] = '\0';
    TokenType type = is_float ? TOKEN_FLOAT : TOKEN_INTEGER;
    return token_create(type, buffer, start_line, start_column);
}

// Read string literal
Token lexer_read_string(Lexer* lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    char buffer[1024];
    int i = 0;
    char quote_char = lexer->current_char;
    
    lexer_advance(lexer); // skip opening quote
    
    while (lexer->current_char != '\0' && lexer->current_char != quote_char && i < 1023) {
        if (lexer->current_char == '\\') {
            lexer_advance(lexer);
            switch (lexer->current_char) {
                case 'n': buffer[i++] = '\n'; break;
                case 't': buffer[i++] = '\t'; break;
                case 'r': buffer[i++] = '\r'; break;
                case '\\': buffer[i++] = '\\'; break;
                case '"': buffer[i++] = '"'; break;
                case '\'': buffer[i++] = '\''; break;
                default: 
                    buffer[i++] = '\\';
                    buffer[i++] = lexer->current_char;
                    break;
            }
        } else {
            buffer[i++] = lexer->current_char;
        }
        lexer_advance(lexer);
    }
    
    if (lexer->current_char == quote_char) {
        lexer_advance(lexer); // skip closing quote
    } else {
        return token_error("Unterminated string literal", start_line, start_column);
    }
    
    buffer[i] = '\0';
    return token_create(TOKEN_STRING, buffer, start_line, start_column);
}

// Read character literal
Token lexer_read_char(Lexer* lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    char buffer[4];
    int i = 0;
    
    lexer_advance(lexer); // skip opening quote
    
    if (lexer->current_char == '\\') {
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);
    } else {
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);
    }
    
    if (lexer->current_char == '\'') {
        lexer_advance(lexer); // skip closing quote
    } else {
        return token_error("Unterminated character literal", start_line, start_column);
    }
    
    buffer[i] = '\0';
    return token_create(TOKEN_CHAR, buffer, start_line, start_column);
}

// Read operator
Token lexer_read_operator(Lexer* lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    char buffer[4];
    int i = 0;
    
    buffer[i++] = lexer->current_char;
    char first = lexer->current_char;
    lexer_advance(lexer);
    
    // Check for two-character operators
    if (lexer->current_char != '\0') {
        char second = lexer->current_char;
        
        // Two character operators
        if ((first == '=' && second == '=') ||
            (first == '!' && second == '=') ||
            (first == '<' && second == '=') ||
            (first == '>' && second == '=') ||
            (first == '&' && second == '&') ||
            (first == '|' && second == '|') ||
            (first == '+' && second == '+') ||
            (first == '-' && second == '-') ||
            (first == '+' && second == '=') ||
            (first == '-' && second == '=') ||
            (first == '*' && second == '=') ||
            (first == '/' && second == '=') ||
            (first == '%' && second == '=') ||
            (first == '<' && second == '<') ||
            (first == '>' && second == '>') ||
            (first == '-' && second == '>') ||
            (first == ':' && second == ':')) {
            buffer[i++] = lexer->current_char;
            lexer_advance(lexer);
        }
    }
    
    buffer[i] = '\0';
    return token_create(TOKEN_OPERATOR, buffer, start_line, start_column);
}

// Read preprocessor directive
Token lexer_read_preprocessor(Lexer* lexer) {
    int start_line = lexer->line;
    int start_column = lexer->column;
    char buffer[256];
    int i = 0;
    
    // Include the # character
    buffer[i++] = lexer->current_char;
    lexer_advance(lexer);
    
    // Read the directive name and arguments
    while (lexer->current_char != '\0' && lexer->current_char != '\n' && i < 255) {
        buffer[i++] = lexer->current_char;
        lexer_advance(lexer);
    }
    
    buffer[i] = '\0';
    return token_create(TOKEN_PREPROCESSOR, buffer, start_line, start_column);
}

// Main tokenization function
Token lexer_next_token(Lexer* lexer) {
    if (!lexer) {
        return token_error("Invalid lexer", 0, 0);
    }
    
    while (lexer->current_char != '\0') {
        // Skip whitespace
        if (is_whitespace(lexer->current_char)) {
            lexer_skip_whitespace(lexer);
            continue;
        }
        
        // Skip comments
        if (lexer->current_char == '/' && 
            (lexer_peek(lexer) == '/' || lexer_peek(lexer) == '*')) {
            lexer_skip_comment(lexer);
            continue;
        }
        
        // Preprocessor directives
        if (lexer->current_char == '#') {
            return lexer_read_preprocessor(lexer);
        }
        
        // String literals
        if (lexer->current_char == '"') {
            return lexer_read_string(lexer);
        }
        
        // Character literals
        if (lexer->current_char == '\'') {
            return lexer_read_char(lexer);
        }
        
        // Numbers
        if (is_digit(lexer->current_char)) {
            return lexer_read_number(lexer);
        }
        
        // Identifiers and keywords
        if (is_alpha(lexer->current_char)) {
            return lexer_read_identifier(lexer);
        }
        
        // Operators
        if (is_operator_char(lexer->current_char)) {
            return lexer_read_operator(lexer);
        }
        
        // Delimiters
        if (is_delimiter_char(lexer->current_char)) {
            char buffer[2] = {lexer->current_char, '\0'};
            Token token = token_create(TOKEN_DELIMITER, buffer, lexer->line, lexer->column);
            lexer_advance(lexer);
            return token;
        }
        
        // Unknown character
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "Unknown character: '%c'", lexer->current_char);
        Token error = token_error(buffer, lexer->line, lexer->column);
        lexer_advance(lexer);
        return error;
    }
    
    // End of file
    return token_create(TOKEN_EOF, NULL, lexer->line, lexer->column);
} 