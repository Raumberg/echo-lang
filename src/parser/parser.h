#ifndef PARSER_H
#define PARSER_H

#include "../lexer/lexer.h"
#include "../ast/ast.h"
#include <stdbool.h>

// Parser structure
typedef struct {
    Lexer* lexer;
    Token current_token;
    Token peek_token;
    bool has_error;
    int error_count;
    char* error_message;
} Parser;

// Parser creation and destruction
Parser* parser_create(Lexer* lexer);
void parser_destroy(Parser* parser);

// Main parsing function
ASTNode* parser_parse(Parser* parser);

// Parsing functions for different constructs
ASTNode* parse_program(Parser* parser);
ASTNode* parse_function(Parser* parser);
ASTNode* parse_struct(Parser* parser);
ASTNode* parse_statement(Parser* parser);
ASTNode* parse_expression(Parser* parser);
ASTNode* parse_type(Parser* parser);
ASTNode* parse_block(Parser* parser);
ASTNode* parse_variable_declaration(Parser* parser);
ASTNode* parse_return_statement(Parser* parser);
ASTNode* parse_if_statement(Parser* parser);
ASTNode* parse_for_statement(Parser* parser);
ASTNode* parse_while_statement(Parser* parser);

// Expression parsing (precedence climbing)
ASTNode* parse_assignment(Parser* parser);
ASTNode* parse_logical_or(Parser* parser);
ASTNode* parse_logical_and(Parser* parser);
ASTNode* parse_equality(Parser* parser);
ASTNode* parse_comparison(Parser* parser);
ASTNode* parse_term(Parser* parser);
ASTNode* parse_factor(Parser* parser);
ASTNode* parse_unary(Parser* parser);
ASTNode* parse_postfix(Parser* parser);
ASTNode* parse_primary(Parser* parser);
ASTNode* parse_call(Parser* parser, ASTNode* callee);

// Utility functions
Token parser_current_token(Parser* parser);
Token parser_peek_token(Parser* parser);
void parser_advance(Parser* parser);
bool parser_match(Parser* parser, TokenType type);
bool parser_check(Parser* parser, TokenType type);
bool parser_expect(Parser* parser, TokenType type, const char* message);
bool parser_expect_keyword(Parser* parser, const char* keyword);

// Error handling
void parser_error(Parser* parser, const char* message);
void parser_synchronize(Parser* parser);
bool parser_has_error(Parser* parser);
const char* parser_get_error(Parser* parser);

// Helper functions
bool is_type_keyword(const char* keyword);
bool is_binary_operator(const char* op);
bool is_unary_operator(const char* op);
int get_operator_precedence(const char* op);

#endif // PARSER_H 