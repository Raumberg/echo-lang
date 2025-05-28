#define _GNU_SOURCE
#include "parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Type keywords
static const char* type_keywords[] = {
    "i8", "i16", "i32", "i64", "f32", "f64", "bool", "string", "char", "void", "auto", NULL
};

// Binary operators with precedence
static const struct {
    const char* op;
    int precedence;
} binary_operators[] = {
    {"||", 1}, {"&&", 2}, {"==", 3}, {"!=", 3}, {"<", 4}, {">", 4}, 
    {"<=", 4}, {">=", 4}, {"+", 5}, {"-", 5}, {"*", 6}, {"/", 6}, {"%", 6}, {NULL, 0}
};

// Unary operators
static const char* unary_operators[] = {
    "!", "-", "+", "*", "&", "++", "--", NULL
};

// Create parser
Parser* parser_create(Lexer* lexer) {
    if (!lexer) return NULL;
    
    Parser* parser = malloc(sizeof(Parser));
    if (!parser) return NULL;
    
    parser->lexer = lexer;
    parser->has_error = false;
    parser->error_count = 0;
    parser->error_message = NULL;
    
    // Initialize tokens
    parser->current_token = lexer_next_token(lexer);
    parser->peek_token = lexer_next_token(lexer);
    
    return parser;
}

// Destroy parser
void parser_destroy(Parser* parser) {
    if (!parser) return;
    
    token_destroy(&parser->current_token);
    token_destroy(&parser->peek_token);
    free(parser->error_message);
    free(parser);
}

// Get current token
Token parser_current_token(Parser* parser) {
    return parser->current_token;
}

// Get peek token
Token parser_peek_token(Parser* parser) {
    return parser->peek_token;
}

// Advance to next token
void parser_advance(Parser* parser) {
    if (!parser) return;
    
    token_destroy(&parser->current_token);
    parser->current_token = parser->peek_token;
    parser->peek_token = lexer_next_token(parser->lexer);
}

// Check if current token matches type
bool parser_check(Parser* parser, TokenType type) {
    return parser && parser->current_token.type == type;
}

// Match and consume token if it matches type
bool parser_match(Parser* parser, TokenType type) {
    if (parser_check(parser, type)) {
        parser_advance(parser);
        return true;
    }
    return false;
}

// Expect specific token type
bool parser_expect(Parser* parser, TokenType type, const char* message) {
    if (parser_check(parser, type)) {
        parser_advance(parser);
        return true;
    }
    
    parser_error(parser, message);
    return false;
}

// Expect specific keyword
bool parser_expect_keyword(Parser* parser, const char* keyword) {
    if (parser_check(parser, TOKEN_KEYWORD) && 
        parser->current_token.value && 
        strcmp(parser->current_token.value, keyword) == 0) {
        parser_advance(parser);
        return true;
    }
    
    char error_msg[256];
    snprintf(error_msg, sizeof(error_msg), "Expected keyword '%s'", keyword);
    parser_error(parser, error_msg);
    return false;
}

// Error handling
void parser_error(Parser* parser, const char* message) {
    if (!parser || !message) return;
    
    parser->has_error = true;
    parser->error_count++;
    
    free(parser->error_message);
    
    char full_message[512];
    snprintf(full_message, sizeof(full_message), 
             "Parse error at line %d, column %d: %s (got '%s')",
             parser->current_token.line, parser->current_token.column,
             message, parser->current_token.value ? parser->current_token.value : "EOF");
    
    parser->error_message = strdup(full_message);
    
    printf("ERROR: %s\n", full_message);
}

// Synchronize after error (panic mode recovery)
void parser_synchronize(Parser* parser) {
    if (!parser) return;
    
    parser->has_error = false;
    
    while (parser->current_token.type != TOKEN_EOF) {
        if (parser->current_token.type == TOKEN_DELIMITER && 
            parser->current_token.value && strcmp(parser->current_token.value, ";") == 0) {
            parser_advance(parser);
            return;
        }
        
        if (parser->current_token.type == TOKEN_KEYWORD) {
            const char* kw = parser->current_token.value;
            if (kw && (strcmp(kw, "fn") == 0 || strcmp(kw, "struct") == 0 || 
                      strcmp(kw, "enum") == 0 || strcmp(kw, "if") == 0 ||
                      strcmp(kw, "for") == 0 || strcmp(kw, "while") == 0 ||
                      strcmp(kw, "return") == 0)) {
                return;
            }
        }
        
        parser_advance(parser);
    }
}

// Check if parser has error
bool parser_has_error(Parser* parser) {
    return parser && parser->error_count > 0;
}

// Get error message
const char* parser_get_error(Parser* parser) {
    return parser ? parser->error_message : NULL;
}

// Helper functions
bool is_type_keyword(const char* keyword) {
    if (!keyword) return false;
    
    for (int i = 0; type_keywords[i]; i++) {
        if (strcmp(keyword, type_keywords[i]) == 0) {
            return true;
        }
    }
    return false;
}

bool is_binary_operator(const char* op) {
    if (!op) return false;
    
    for (int i = 0; binary_operators[i].op; i++) {
        if (strcmp(op, binary_operators[i].op) == 0) {
            return true;
        }
    }
    return false;
}

bool is_unary_operator(const char* op) {
    if (!op) return false;
    
    for (int i = 0; unary_operators[i]; i++) {
        if (strcmp(op, unary_operators[i]) == 0) {
            return true;
        }
    }
    return false;
}

int get_operator_precedence(const char* op) {
    if (!op) return 0;
    
    for (int i = 0; binary_operators[i].op; i++) {
        if (strcmp(op, binary_operators[i].op) == 0) {
            return binary_operators[i].precedence;
        }
    }
    return 0;
}

// Main parsing function
ASTNode* parser_parse(Parser* parser) {
    if (!parser) return NULL;
    
    return parse_program(parser);
} 